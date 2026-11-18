"""Exercise actual HTTP workflows against a disposable, isolated data directory."""
import concurrent.futures, csv, datetime, io, json, os, pathlib, socket, subprocess, tempfile, time, urllib.error, urllib.request
ROOT = pathlib.Path(__file__).resolve().parents[1]
with socket.socket() as sock:
    sock.bind(('127.0.0.1', 0)); PORT = sock.getsockname()[1]
BASE = f'http://127.0.0.1:{PORT}'
process = None

def call(path, method='GET', body=None, token=None, expected=200):
    data = json.dumps(body).encode() if body is not None else None
    headers = {'Content-Type': 'application/json'}
    if token: headers['Authorization'] = 'Bearer ' + token
    request = urllib.request.Request(BASE + path, data=data, headers=headers, method=method)
    try: response = urllib.request.urlopen(request, timeout=15)
    except urllib.error.HTTPError as error: response = error
    raw = response.read().decode()
    assert response.status == expected, (method, path, response.status, raw)
    return json.loads(raw) if 'application/json' in response.headers.get('Content-Type','') else raw

def login(username, password): return call('/api/auth/login','POST',dict(username=username,password=password))['token']

def start(directory, log):
    global process
    process = subprocess.Popen([str(ROOT/'build/bloodline_server'),directory,str(ROOT/'frontend'),str(PORT)],stdout=log,stderr=log,env={**os.environ,'TZ':'Asia/Kolkata'})
    for _ in range(60):
        try:
            if call('/api/health')['status']=='ok': return
        except (OSError, AssertionError): pass
        time.sleep(.1)
    log.seek(0)
    raise AssertionError('Test server did not start: ' + log.read().decode())

with tempfile.TemporaryDirectory(prefix='bloodline-flows-') as directory, tempfile.TemporaryFile() as log:
    try:
        start(directory,log)
        admin=login('admin','admin123'); org=login('ngo_admin','123')
        call('/api/auth/register','POST',dict(username='badadmin',fullName='Escalation',password='password123',role='admin'),expected=400)
        call('/api/camps','POST',{},expected=401)
        call('/api/auth/register','POST',{},expected=400)
        pending=call('/api/auth/register','POST',dict(username='pendingorg',fullName='New Organization',password='password123',role='ngo',city='Nainital'),expected=201)['token']
        call('/api/inventory/stock',token=pending,expected=403)
        call('/api/admin/orgs/pendingorg/status','PUT',{'status':'Approved'},admin)
        call('/api/inventory/stock',token=pending)
        print('PASS authentication, malformed input, role isolation, organization approval')
        donor=call('/api/auth/register','POST',dict(username='workflow_donor',fullName='Donor, "One"',password='password123',role='donor',city='Nainital'),expected=201)['token']
        profile=dict(age=28,gender='Male',bloodGroup='O-',weight=70,phone='9876543210',consent=True)
        call('/api/donor/profile','PUT',profile,donor)
        invalid={**profile,'age':35,'weight':1}
        call('/api/donor/profile','PUT',invalid,donor,expected=400)
        assert call('/api/donor/profile',token=donor)['age']==28
        today=datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=5,minutes=30))).date().isoformat()
        campbody=dict(name='Community, "Hope" Camp',date=today,targetUnits=20,slotCapacity=1)
        call('/api/camps','POST',{**campbody,'date':'2026-02-30'},org,expected=400)
        call('/api/camps','POST',{**campbody,'date':'2020-01-01'},org,expected=400)
        camp=call('/api/camps','POST',campbody,org,expected=201)['id']
        call(f'/api/camps/{camp}/register','POST',token=donor)
        call(f'/api/camps/{camp}/register','POST',token=donor,expected=409)
        call(f'/api/camps/{camp}/register','DELETE',token=donor)
        call(f'/api/camps/{camp}/register','POST',token=donor)
        call(f'/api/camps/{camp}/status','PUT',{'status':'nonsense'},org,expected=400)
        call(f'/api/camps/{camp}/status','PUT',{'status':'Open'},org)
        print('PASS profile validation, date validation, booking, duplicate booking, cancellation')
        request=call('/api/requests','POST',dict(bloodGroup='O-',unitsNeeded=1,hospitalName='Community Hospital',urgency='Critical'),org,expected=201)['id']
        assert call(f'/api/requests/{request}/respond','POST',token=donor)['status']=='Open'
        assert call(f'/api/org/requests/{request}/responders',token=org)['responders']
        record=dict(username='workflow_donor',campId=camp,date=today,status='Completed')
        unit=call('/api/donations','POST',record,org,expected=201)['id']
        call('/api/donations','POST',record,org,expected=409)
        call(f'/api/inventory/units/{unit}/status','PUT',{'status':'Transfused'},org)
        call(f'/api/inventory/units/{unit}/status','PUT',{'status':'Completed'},org,expected=400)
        inventory=call('/api/inventory/stock',token=org)
        assert any(u['id']==unit and u['viability']=='Transfused' for u in inventory['expiredUnits'])
        call(f'/api/requests/{request}/status','PUT',{'status':'Fulfilled'},org)
        call(f'/api/camps/{camp}/status','PUT',{'status':'Completed'},org)
        assert call('/api/donor/history',token=donor)
        exported=list(csv.DictReader(io.StringIO(call('/api/org/export/donors.csv',token=org))))
        assert next(r for r in exported if r['Username']=='workflow_donor')['Full Name']=='Donor, \"One\"'
        print('PASS matching pledges, donation recording, inventory lifecycle, request completion, CSV export')
        with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
            assert all(pool.map(lambda _: call('/api/health')['status']=='ok',range(30)))
        process.terminate();process.wait(timeout=10)
        start(directory,log)
        donor=login('workflow_donor','password123');org=login('ngo_admin','123')
        assert call('/api/me',token=donor)['fullName']=='Donor, "One"'
        assert next(c for c in call('/api/camps') if c['id']==camp)['name']=='Community, "Hope" Camp'
        assert call('/api/donor/card',token=donor)['donationCount']==1
        assert call('/api/donor/profile',token=donor)['lastDonationDate']==today
        print('PASS concurrent reads, restart, quoted CSV round-trip, donation history after transfusion')
        print('All HTTP workflow checks passed; live data was not used.')
    finally:
        if process and process.poll() is None:
            process.terminate();process.wait(timeout=10)

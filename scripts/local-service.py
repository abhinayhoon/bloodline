#!/usr/bin/env python3
"""Manage the macOS localhost service without tying it to an open terminal."""
import json, os, pathlib, plistlib, subprocess, sys, time, urllib.request
ROOT = pathlib.Path(__file__).resolve().parents[1]
LABEL = 'com.bloodline.local'
DOMAIN = f'gui/{os.getuid()}'
PLIST = pathlib.Path.home() / 'Library/LaunchAgents' / f'{LABEL}.plist'
RUNTIME = ROOT / '.runtime'

def health():
    try:
        with urllib.request.urlopen('http://127.0.0.1:8080/api/health', timeout=2) as response:
            return json.load(response).get('service') == 'bloodline'
    except Exception:
        return False

def run(*args, check=True):
    return subprocess.run(args, check=check, capture_output=True, text=True)

command = sys.argv[1] if len(sys.argv) > 1 else 'start'
if command == 'status':
    print('BloodLine is running: http://localhost:8080' if health() else 'BloodLine is offline. Run npm start.')
    sys.exit(0 if health() else 1)
if command == 'stop':
    run('launchctl', 'bootout', f'{DOMAIN}/{LABEL}', check=False)
    if PLIST.exists(): PLIST.unlink()
    print('BloodLine stopped. Login startup disabled.')
    sys.exit(0)
if command not in ('start', 'restart'):
    sys.exit('Usage: local-service.py start|stop|restart|status')
if command == 'start' and health():
    print('BloodLine is already running: http://localhost:8080')
    sys.exit(0)
if not (ROOT / 'build/bloodline_server').is_file():
    sys.exit('Build the application first: npm run build')
RUNTIME.mkdir(exist_ok=True)
PLIST.parent.mkdir(parents=True, exist_ok=True)
config = {
    'Label': LABEL,
    'ProgramArguments': [str(ROOT / 'build/bloodline_server'), str(ROOT / 'data'), str(ROOT / 'frontend'), '8080'],
    'WorkingDirectory': str(ROOT), 'RunAtLoad': True, 'KeepAlive': True, 'ThrottleInterval': 5,
    'StandardOutPath': str(RUNTIME / 'server.log'), 'StandardErrorPath': str(RUNTIME / 'server-error.log'),
    'EnvironmentVariables': {'TZ': 'Asia/Kolkata'},
}
PLIST.write_bytes(plistlib.dumps(config))
run('launchctl', 'bootout', f'{DOMAIN}/{LABEL}', check=False)
result = run('launchctl', 'bootstrap', DOMAIN, str(PLIST), check=False)
if result.returncode: sys.exit(result.stderr.strip())
for _ in range(30):
    if health():
        print('BloodLine is running: http://localhost:8080')
        print(f'Logs: {RUNTIME}\nManaged by launchd; restarts automatically and starts at login. Stop with npm stop.')
        break
    time.sleep(.3)
else:
    sys.exit(f'Startup failed. Check {RUNTIME / "server-error.log"}')

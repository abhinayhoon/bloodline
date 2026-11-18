/** Workflow presentation and resilience for the BloodLine local application. */
const Workspace = {
    today() { const d = new Date(); return `${d.getFullYear()}-${String(d.getMonth()+1).padStart(2,'0')}-${String(d.getDate()).padStart(2,'0')}`; },
    empty(title, message) { return `<div class="empty-state"><span aria-hidden="true">♡</span><h3>${escapeHtml(title)}</h3><p>${escapeHtml(message)}</p></div>`; },
    async refreshSession() {
        try { API.setCurrentUser(await API.request('/api/me')); await UI.updateAuthState(); }
        catch(error) { Toast.show(error.message, 'error'); }
    },
    async health() {
        try {
            const response = await fetch('/api/health', { signal: AbortSignal.timeout(3000), cache: 'no-store' });
            if (!response.ok) throw new Error('Offline');
            this.online(true);
        } catch (_) { this.online(false); }
    },
    online(ok) {
        document.querySelectorAll('.connection-status').forEach(el => { el.textContent = ok ? 'Connected to BloodLine' : 'Connection unavailable'; el.classList.toggle('offline', !ok); });
        const banner = document.getElementById('connection-banner');
        banner.hidden = ok;
        banner.innerHTML = ok ? '' : 'Unable to reach BloodLine. Your form entries are still here. <button type="button" onclick="Workspace.health()">Retry connection ↻</button>';
    },
    async summary() {
        const el = document.getElementById('home-summary');
        try {
            const [camps, stock, requests] = await Promise.all([API.request('/api/camps'), API.request('/api/public/stock'), API.request('/api/requests')]);
            const upcoming = camps.filter(c => c.date >= this.today() && !['Completed','Cancelled'].includes(c.status));
            const units = Object.values(stock).reduce((sum, group) => sum + group.count, 0);
            el.innerHTML = [[upcoming.length,'Upcoming camps'],[units,'Available blood units'],[requests.filter(r=>r.status==='Open').length,'Open blood requests']].map(([number,label])=>`<article><span>${label}</span><strong>${number}</strong></article>`).join('');
            const next = upcoming.sort((a,b)=>a.date.localeCompare(b.date)).slice(0,3);
            document.getElementById('home-upcoming').innerHTML = next.length ? next.map(c=>`<article><span class="date-label">${escapeHtml(c.date)}</span><h3>${escapeHtml(c.name)}</h3><p>${c.remainingSlots} places available · ${escapeHtml(c.status)}</p><button class="text-action" onclick="Navigation.goTo('camps-section')">View camp ↗</button></article>`).join('') : this.empty('Your next opportunity is on its way','No upcoming camps are scheduled. Check all camps for previous drives, or sign in as an organization to schedule one.');
        } catch (_) { el.innerHTML = this.empty('Community data is unavailable','Check your connection and try again.'); }
    },
    printCard() { document.body.classList.add('printing-card'); window.print(); },
    init() {
        document.querySelectorAll('.workspace-rail button').forEach(button=>button.addEventListener('click',()=>{
            button.closest('.workspace-rail').querySelectorAll('button').forEach(b=>b.setAttribute('aria-current', String(b===button)));
        }));
        document.getElementById('camp-search').addEventListener('input',()=>{clearTimeout(this.searchTimer);this.searchTimer=setTimeout(()=>CampsManager.renderCamps(),180);});
        document.getElementById('camp-filter').addEventListener('change',()=>CampsManager.renderCamps());
        ['inv-filter-group','inv-filter-status'].forEach(id=>document.getElementById(id).addEventListener('change',()=>NGODashboard.filterInventory()));
        ['camp-new-date','mec-date'].forEach(id=>document.getElementById(id).min=this.today());
        const donationDate=document.getElementById('don-date'); donationDate.value=this.today(); donationDate.max=this.today();
        ['camp-new-target','camp-new-slots','req-units'].forEach(id=>{const input=document.getElementById(id);input.min='1';input.max='10000';input.required=true;});
        document.getElementById('don-camp').required=true;
        document.getElementById('don-camp').addEventListener('change', event => {
            const camp = CampsManager.camps.find(c => c.id === event.target.value);
            if (camp) document.getElementById('don-date').value = camp.date;
            NGODashboard.onCampSelected(event.target.value);
        });
        document.getElementById('reg-username').pattern='[A-Za-z0-9_]{3,30}';
        document.getElementById('reg-username').title='3–30 letters, numbers, or underscores';
        document.getElementById('reg-password').minLength=3;
        document.querySelectorAll('.modal-overlay').forEach(modal=>modal.addEventListener('click',e=>{if(e.target===modal) modal.classList.remove('open');}));
        addEventListener('afterprint',()=>document.body.classList.remove('printing-card'));
        addEventListener('online',()=>this.health());
        addEventListener('offline',()=>this.online(false));
        document.addEventListener('visibilitychange',()=>{if(!document.hidden)this.health();});
        setInterval(()=>{if(!document.hidden)this.health();},15000);
        this.health(); this.summary();
    }
};

// Return meaningful connection failures without discarding a valid session on a timeout.
const originalRequest = API.request.bind(API);
API.request = async function(endpoint, options={}) {
    const form = Workspace.submittingForm;
    try { const result = await originalRequest(endpoint, options); Workspace.online(true); return result; }
    catch(error) {
        if (error.name === 'TimeoutError' || error.name === 'AbortError' || error instanceof TypeError) {
            Workspace.online(false);
            throw new Error('Connection interrupted. Your entries are preserved. Check the connection before retrying.');
        }
        throw error;
    } finally {
        if(form) { delete form.dataset.submitting; form.querySelectorAll('[type="submit"], button:not([type])').forEach(b=>b.disabled=false); if(Workspace.submittingForm===form)Workspace.submittingForm=null; }
    }
};
document.addEventListener('submit', event=>{
    const form=event.target;
    if(form.dataset.submitting){event.preventDefault();event.stopImmediatePropagation();return;}
    form.dataset.submitting='true'; Workspace.submittingForm=form;
    setTimeout(()=>{delete form.dataset.submitting;if(Workspace.submittingForm===form)Workspace.submittingForm=null;},13000);
},true);

// Local task screens remain usable without Wikipedia, weather, map, or avatar services.
LiveInternetFeeds.init = () => {};
LiveInternetFeeds.fetchWikiBloodBank = async () => {};
LiveInternetFeeds.fetchWikiRedCross = async () => {};
LiveInternetFeeds.fetchWikiTransfusion = async () => {};
LiveInternetFeeds.fetchHealthBulletins = async () => {};
LiveInternetFeeds.onCampsSectionShown = () => {};
LiveInternetFeeds.updateMapMarkers = () => {};

CampsManager.renderCamps = async function() {
    const grid = document.getElementById('camps-grid');
    try {
        const camps = await API.request('/api/camps');
        this.camps = camps; this.populateDropdown();
        const search = document.getElementById('camp-search').value.trim().toLowerCase();
        const mode = document.getElementById('camp-filter').value;
        const finished = c => c.date < Workspace.today() || ['Completed','Cancelled'].includes(c.status);
        const filtered = camps.filter(c=>(!search || c.name.toLowerCase().includes(search)) && (mode==='all' || (mode==='past' ? finished(c) : !finished(c)))).sort((a,b)=>a.date.localeCompare(b.date));
        if(!filtered.length){grid.innerHTML=Workspace.empty('No camps to show',mode==='upcoming'?'No upcoming camps match this view. Choose “All camps” to explore past drives.':'Try another search or filter.');return;}
        const user=API.getCurrentUser();
        grid.innerHTML=filtered.map(c=>{
            const booked=c.isRegistered || c.registeredDonors?.includes(user?.username);
            let action='<span class="muted">Registration ended</span>';
            if(!finished(c)) {
                if(!user)action=`<button class="btn btn-primary" onclick="Auth.openModal('login')">Sign in to reserve ↗</button>`;
                else if(user.role==='donor')action=booked?`<span class="badge badge-open">✓ Reserved</span><button class="text-action" onclick="CampsManager.cancelRegistration('${c.id}')">Cancel reservation</button>`:c.remainingSlots>0&&c.status!=='Full'?`<button class="btn btn-primary" onclick="CampsManager.registerDonor('${c.id}')">Reserve a place ↗</button>`:'<span class="badge badge-urgent">Camp is full</span>';
                else if(user.role==='ngo' && user.approvalStatus==='Approved')action=`<button class="btn btn-outline" onclick="Navigation.goTo('dashboard-ngo')">Manage in workspace ↗</button>`;
            }
            const {month,day}=this.parseDate(c.date);
            return `<article class="drive-card"><div class="drive-card-top"><div class="drive-date"><strong>${day}</strong><span>${month} ${c.date.slice(0,4)}</span></div><span class="badge badge-${escapeHtml(c.status.toLowerCase())}">${escapeHtml(c.status)}</span></div><h3>${escapeHtml(c.name)}</h3><p>${c.remainingSlots} places remaining · ${c.slotCapacity} total capacity</p><div class="drive-progress"><span>${c.unitsCollected} / ${c.targetUnits} units collected</span><progress value="${c.unitsCollected}" max="${c.targetUnits}"></progress></div><div class="drive-actions">${action}</div></article>`;
        }).join('');
    } catch(error) { grid.innerHTML=Workspace.empty('Camps could not be loaded',error.message)+'<button class="btn btn-outline" onclick="CampsManager.renderCamps()">Try again ↻</button>'; }
};
CampsManager.populateDropdown = function() {
    const select=document.getElementById('don-camp');const previous=select.value;
    select.innerHTML='<option value="">Choose an open camp</option>'+this.camps.filter(c=>['Open','Full'].includes(c.status)).map(c=>`<option value="${c.id}">${escapeHtml(c.name)} · ${escapeHtml(c.date)}</option>`).join('');
    select.value=previous;
};
UI.updateAuthState = async function() {
    const user=API.getCurrentUser();const logged=!!user&&!!API.getToken();
    document.getElementById('btn-login-nav').style.display=logged?'none':'inline-flex';
    document.getElementById('btn-logout-nav').style.display=logged?'inline-flex':'none';
    ['donor','ngo','admin'].forEach(role=>document.getElementById('nav-'+role).style.display=logged&&user.role===role?'inline-block':'none');
    if(!logged) { Workspace.lastUser = null; return; }
    if (Workspace.lastUser !== user.username) {
        document.querySelectorAll('#dashboard-donor form, #dashboard-ngo form').forEach(form => form.reset());
        document.getElementById('don-date').value = Workspace.today();
        Workspace.lastUser = user.username;
    }
    if(user.role==='admin') {document.getElementById('dash-admin-name').textContent=user.fullName;await Promise.all([AdminDashboard.loadOrgs(),AdminDashboard.loadAudit(1)]);}
    else if(user.role==='ngo') {
        document.getElementById('dash-ngo-name').textContent=user.fullName;
        const pending=user.approvalStatus!=='Approved';const notice=document.getElementById('ngo-notice');notice.hidden=!pending;
        notice.innerHTML=`<p>Organization status: ${escapeHtml(user.approvalStatus)}. An administrator must approve your organization before you can manage camps, records, or donor data.</p><button class="btn btn-outline" type="button" onclick="Workspace.refreshSession()">Check approval status ↻</button>`;
        document.querySelector('#dashboard-ngo .workspace-layout').hidden=pending;
        if(!pending)await Promise.all([NGODashboard.loadKPIs(),NGODashboard.loadCampsTable(),NGODashboard.loadInventory(),NGODashboard.loadProfileForm(),NGODashboard.setupDonorAutocomplete()]);
    } else {
        await DonorDashboard.loadProfile();
        await Promise.all([DonorDashboard.loadCard(),DonorDashboard.loadAppointments(),DonorDashboard.loadMatchingRequests(),DonorDashboard.loadPledgedRequests(),DonorDashboard.renderHistory()]);
    }
};
const navigate=Navigation.goTo.bind(Navigation);
Navigation.goTo=async function(id) {
    const user=API.getCurrentUser();
    if(id.startsWith('dashboard-')&&(!user||id!=='dashboard-'+user.role))id='auth-section';
    if(!document.getElementById(id)?.classList.contains('page-section'))id='home-section';
    if(location.hash!=='#'+id)history.pushState(null,'','#'+id);
    await navigate(id);
    document.querySelectorAll('.nav-links a').forEach(a=>{if(a.dataset.target===id)a.setAttribute('aria-current','page');else a.removeAttribute('aria-current');});
};
const initialize=UI.init.bind(UI);
UI.init=async function(){await initialize();Workspace.init();if(location.hash)await Navigation.goTo(location.hash.slice(1));};
addEventListener('popstate',()=>Navigation.goTo(location.hash.slice(1)||'home-section'));

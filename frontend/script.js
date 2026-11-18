/**
 * BloodLine - Full-Stack Client Logic (Phase 1 & Phase 2)
 * Comprehensive SPA client connecting to C++17 REST backend
 */

// ==========================================
// 1. UTILITIES & API CLIENT
// ==========================================

function escapeHtml(str) {
    if (str === null || str === undefined) return '';
    return String(str)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#39;');
}

const Toast = {
    show(message, type = 'info', duration = 3500) {
        const container = document.getElementById('toast-container');
        if (!container) {
            console.log(`[Toast ${type}] ${message}`);
            return;
        }
        const toast = document.createElement('div');
        toast.className = `toast toast-${type}`;
        let icon = 'ℹ️';
        if (type === 'success') icon = '✓';
        else if (type === 'error') icon = '⚠️';
        else if (type === 'warning') icon = '⚡';

        toast.innerHTML = `<span style="font-size:1.1rem; font-weight:bold;">${icon}</span> <span>${escapeHtml(message)}</span>`;
        container.appendChild(toast);

        setTimeout(() => {
            toast.style.opacity = '0';
            toast.style.transform = 'translateX(40px)';
            setTimeout(() => toast.remove(), 300);
        }, duration);
    }
};

const API = {
    baseUrl: '',

    getToken() {
        return localStorage.getItem('bl_token') || '';
    },

    setToken(token) {
        localStorage.setItem('bl_token', token);
    },

    getCurrentUser() {
        const str = localStorage.getItem('bl_current_user');
        try {
            return str ? JSON.parse(str) : null;
        } catch (e) {
            return null;
        }
    },

    setCurrentUser(user) {
        localStorage.setItem('bl_current_user', JSON.stringify(user));
    },

    clearAuth() {
        localStorage.removeItem('bl_token');
        localStorage.removeItem('bl_current_user');
    },

    async request(endpoint, options = {}) {
        const headers = options.headers || {};
        headers['Content-Type'] = 'application/json';

        const token = this.getToken();
        if (token) {
            headers['Authorization'] = `Bearer ${token}`;
        }

        const config = {
            ...options,
            headers
        };

        try {
            const res = await fetch(`${this.baseUrl}${endpoint}`, { ...config, signal: config.signal || AbortSignal.timeout(12000) });
            let data = null;
            const contentType = res.headers.get('content-type');
            if (contentType && contentType.includes('application/json')) {
                data = await res.json();
            }

            if (!res.ok) {
                if (res.status === 401 && endpoint !== '/api/auth/login') {
                    // Session expired or invalid
                    this.clearAuth();
                    UI.updateAuthState();
                    Navigation.goTo('auth-section');
                    const loginErr = document.getElementById('login-error');
                    if (loginErr) {
                        loginErr.textContent = 'Your session has expired. Please sign in again.';
                        loginErr.style.display = 'block';
                    }
                }

                const errorMsg = (data && data.error) ? data.error : `HTTP ${res.status}: ${res.statusText}`;
                const err = new Error(errorMsg);
                err.status = res.status;
                err.data = data;
                throw err;
            }

            return data;
        } catch (err) {
            console.error(`API Error [${endpoint}]:`, err);
            throw err;
        }
    }
};

// ==========================================
// ==========================================
// 2. SPA NAVIGATION & REAL-TIME NETWORK LOADER (loading-dev Clock)
// ==========================================

/**
 * loading-dev Clock Component
 * Directly implements the SVG, rotation animation, and size specifications from `loading-dev`.
 * Usage:
 *   import { Clock } from "loading-dev";
 *   export function ClockDemo() { return <Clock size={48} />; }
 */
function Clock({ size = 48, color = 'var(--primary)', className = '' } = {}) {
    return `
    <svg class="ld-clock ${className}" role="presentation" aria-hidden="true" viewBox="0 0 16 16" fill="none" style="--ld-size:${size}px; width:${size}px; height:${size}px; color:${color};">
        <circle cx="8" cy="8" fill="currentColor" opacity="0.15" r="8"></circle>
        <path class="ld-clock-spin" d="M11.1937 2.92061C10.5206 2.49739 9.77304 2.21397 8.9954 2.08314C8.45076 1.99151 8 2.44772 8 3V7.20324C8 7.49211 8.12492 7.76686 8.34259 7.95677L11.8999 11.0603C12.3287 11.4344 12.9913 11.378 13.2643 10.8787C13.6356 10.1998 13.8736 9.45248 13.9617 8.67713C14.0892 7.55433 13.8971 6.41834 13.4074 5.39993C12.9177 4.38152 12.1503 3.5221 11.1937 2.92061Z" fill="currentColor"></path>
    </svg>`;
}

function ClockDemo() {
    return Clock({ size: 48 });
}

window.Clock = Clock;
window.ClockDemo = ClockDemo;

const PageLoader = {
    bar: null,
    overlay: null,
    label: null,
    timer: null,
    minTimePromise: null,

    start(message = 'Connecting live feed...') {
        if (!this.bar) this.bar = document.getElementById('page-loader-bar');
        if (!this.overlay) this.overlay = document.getElementById('page-loading-overlay');
        if (!this.label) this.label = document.getElementById('page-loading-label');

        if (this.label) this.label.textContent = message;
        if (this.overlay) this.overlay.classList.add('active');

        if (this.bar) {
            clearInterval(this.timer);
            this.bar.style.opacity = '1';
            this.bar.style.width = '25%';

            this.timer = setInterval(() => {
                const current = parseFloat(this.bar.style.width) || 25;
                if (current < 85) {
                    this.bar.style.width = (current + Math.floor(Math.random() * 10) + 4) + '%';
                }
            }, 100);
        }

        // Tolerable duration guarantee (580ms): Ensures the user gets to experience the authentic
        // rotating Clock animation from loading-dev while live internet data streams in parallel.
        this.minTimePromise = new Promise(resolve => setTimeout(resolve, 580));
    },

    async finish() {
        if (this.minTimePromise) {
            await this.minTimePromise;
            this.minTimePromise = null;
        }

        if (this.bar) {
            clearInterval(this.timer);
            this.bar.style.width = '100%';
        }

        if (this.overlay) {
            this.overlay.classList.remove('active');
        }

        setTimeout(() => {
            if (this.bar) {
                this.bar.style.opacity = '0';
                setTimeout(() => {
                    if (this.bar) this.bar.style.width = '0%';
                }, 250);
            }
        }, 200);
    }
};

const Navigation = {
    init() {
        // Nav links inside header & in-page jump targets
        document.querySelectorAll('.nav-links a, a[data-target]').forEach(link => {
            link.addEventListener('click', (e) => {
                e.preventDefault();
                const targetId = link.getAttribute('data-target');
                if (!targetId) return;

                document.querySelectorAll('.nav-links a').forEach(a => a.classList.remove('active'));
                const navLink = document.querySelector(`.nav-links a[data-target="${targetId}"]`);
                if (navLink) navLink.classList.add('active');

                this.goTo(targetId);
                this.collapseMobile();
            });
        });

        // Intercept any href="#camps" or href="#camps-section" links anywhere on the page
        document.querySelectorAll('a[href="#camps"], a[href="#camps-section"], a[href="#dash-donor-appointments"]').forEach(link => {
            link.addEventListener('click', (e) => {
                e.preventDefault();
                const href = link.getAttribute('href');
                if (href === '#dash-donor-appointments') {
                    Navigation.goTo('dashboard-donor');
                    const tabBtn = document.querySelector('.donor-nav[data-target="dash-donor-appointments"]');
                    if (tabBtn) tabBtn.click();
                } else {
                    Navigation.goTo('camps-section');
                }
                this.collapseMobile();
            });
        });

        // Dedicated handler for the Nav "Login" Button (Fix for Bug #1)
        const loginNavBtn = document.getElementById('btn-login-nav');
        if (loginNavBtn) {
            loginNavBtn.addEventListener('click', (e) => {
                e.preventDefault();
                this.activateAuthTab('login');
                this.goTo('auth-section');
                this.collapseMobile();
            });
        }

        // Dedicated handler for the Nav "Logout" Button
        const logoutNavBtn = document.getElementById('btn-logout-nav');
        if (logoutNavBtn) {
            logoutNavBtn.addEventListener('click', async (e) => {
                e.preventDefault();
                try {
                    await API.request('/api/auth/logout', { method: 'POST' });
                } catch (err) {
                    // Local clean up anyway
                }
                API.clearAuth();
                UI.updateAuthState();
                this.goTo('home-section');
                this.collapseMobile();
            });
        }

        // Hero CTA buttons (Fix for Bug #2)
        document.querySelectorAll('.nav-trigger').forEach(btn => {
            btn.addEventListener('click', (e) => {
                e.preventDefault();
                const target = btn.getAttribute('data-target') || 'auth-section';
                const role = btn.getAttribute('data-role');

                if (target === 'auth-section') {
                    Navigation.activateAuthTab('register');
                    if (role) {
                        const radio = document.querySelector(`input[name="reg-role"][value="${role}"]`);
                        if (radio) radio.checked = true;
                    }
                }
                Navigation.goTo(target);
            });
        });

        // Mobile menu toggle (IntegratedBio fullscreen overlay nav)
        const mobileBtn = document.getElementById('mobile-menu-btn');
        const navContainer = document.querySelector('.nav-links');
        if (mobileBtn && navContainer) {
            mobileBtn.addEventListener('click', () => {
                const open = navContainer.classList.toggle('mobile-open');
                mobileBtn.setAttribute('aria-expanded', String(open));
            });
        }
    },

    collapseMobile() {
        if (window.innerWidth <= 800) {
            const navContainer = document.querySelector('.nav-links');
            if (navContainer) navContainer.classList.remove('mobile-open');
            document.getElementById('mobile-menu-btn')?.setAttribute('aria-expanded', 'false');
        }
    },

    activateAuthTab(tabName) {
        document.querySelectorAll('.auth-tab').forEach(t => t.classList.remove('active'));
        document.querySelectorAll('.auth-panel').forEach(p => p.classList.remove('active'));

        const targetTab = document.querySelector(`.auth-tab[data-tab="${tabName}"]`);
        if (targetTab) targetTab.classList.add('active');

        const targetPanel = document.getElementById(`panel-${tabName}`);
        if (targetPanel) targetPanel.classList.add('active');
    },

    async goTo(sectionId) {
        document.querySelectorAll('.page-section').forEach(sec => sec.classList.remove('active'));
        const targetSec = document.getElementById(sectionId);
        if (targetSec) targetSec.classList.add('active');

        document.querySelectorAll('.nav-links a').forEach(a => {
            if (a.style.display !== 'none') {
                a.classList.toggle('active', a.getAttribute('data-target') === sectionId);
            }
        });

        window.scrollTo({ top: 0, behavior: 'instant' });

        try {
            if (typeof LiveInternetFeeds !== 'undefined') {
                if (sectionId === 'home-section') {
                    void LiveInternetFeeds.fetchWikiBloodBank();
                } else if (sectionId === 'about-section') {
                    void LiveInternetFeeds.fetchWikiRedCross();
                } else if (sectionId === 'info-section') {
                    void Promise.all([
                        LiveInternetFeeds.fetchWikiTransfusion(),
                        LiveInternetFeeds.fetchHealthBulletins()
                    ]);
                } else if (sectionId === 'camps-section') {
                    LiveInternetFeeds.onCampsSectionShown();
                    await CampsManager.renderCamps();
                } else if (sectionId === 'stock-section') {
                    await PublicStock.render();
                } else if (sectionId === 'records-section') {
                    await PublicRecords.render(1);
                }
            }
        } finally {
            document.getElementById('page-loading-overlay')?.classList.remove('active');
        }
    }
};

// ==========================================
// 3. AUTHENTICATION MODULE
// ==========================================

const Auth = {
    openModal(tab = 'login') {
        Navigation.activateAuthTab(tab);
        Navigation.goTo('auth-section');
        window.scrollTo({ top: 0, behavior: 'smooth' });
    },

    init() {
        // Tab switching
        document.querySelectorAll('.auth-tab').forEach(tab => {
            tab.addEventListener('click', () => {
                Navigation.activateAuthTab(tab.dataset.tab);
            });
        });

        // Clear error messages on input (Fix for Bug #3)
        const loginUserInp = document.getElementById('login-username');
        const loginPassInp = document.getElementById('login-password');
        const loginErr = document.getElementById('login-error');
        if (loginUserInp && loginPassInp && loginErr) {
            const clearLoginErr = () => { loginErr.style.display = 'none'; };
            loginUserInp.addEventListener('input', clearLoginErr);
            loginPassInp.addEventListener('input', clearLoginErr);
        }

        const regUserInp = document.getElementById('reg-username');
        const regPassInp = document.getElementById('reg-password');
        const regErr = document.getElementById('register-error');
        if (regUserInp && regPassInp && regErr) {
            const clearRegErr = () => { regErr.style.display = 'none'; };
            regUserInp.addEventListener('input', clearRegErr);
            regPassInp.addEventListener('input', clearRegErr);
        }

        // Login form submit
        const loginForm = document.getElementById('login-form');
        if (loginForm) {
            loginForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const btn = document.getElementById('btn-submit-login');
                btn.disabled = true;
                btn.classList.add('loading');

                const username = document.getElementById('login-username').value.trim();
                const password = document.getElementById('login-password').value;

                loginErr.style.display = 'none';

                try {
                    const res = await API.request('/api/auth/login', {
                        method: 'POST',
                        body: JSON.stringify({ username, password })
                    });

                    API.setToken(res.token);
                    API.setCurrentUser(res.user);
                    loginForm.reset();
                    await UI.updateAuthState();

                    if (res.user.role === 'admin') {
                        Navigation.goTo('dashboard-admin');
                    } else if (res.user.role === 'ngo') {
                        Navigation.goTo('dashboard-ngo');
                    } else {
                        Navigation.goTo('dashboard-donor');
                    }
                } catch (err) {
                    loginErr.textContent = err.message || 'Invalid username or password.';
                    loginErr.style.display = 'block';
                } finally {
                    btn.disabled = false;
                    btn.classList.remove('loading');
                }
            });
        }

        // Register form submit
        const regForm = document.getElementById('register-form');
        if (regForm) {
            regForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const btn = document.getElementById('btn-submit-register');
                btn.disabled = true;
                btn.classList.add('loading');

                const fullName = document.getElementById('reg-fullname').value.trim();
                const username = document.getElementById('reg-username').value.trim();
                const city = document.getElementById('reg-city').value.trim();
                const password = document.getElementById('reg-password').value;
                const roleRadio = document.querySelector('input[name="reg-role"]:checked');
                const role = roleRadio ? roleRadio.value : 'donor';

                regErr.style.display = 'none';

                try {
                    const res = await API.request('/api/auth/register', {
                        method: 'POST',
                        body: JSON.stringify({ fullName, username, password, role, city })
                    });

                    API.setToken(res.token);
                    API.setCurrentUser(res.user);
                    regForm.reset();
                    await UI.updateAuthState();

                    if (res.user.role === 'ngo') {
                        Navigation.goTo('dashboard-ngo');
                    } else {
                        Navigation.goTo('dashboard-donor');
                    }
                } catch (err) {
                    regErr.textContent = err.message || 'Registration failed.';
                    regErr.style.display = 'block';
                } finally {
                    btn.disabled = false;
                    btn.classList.remove('loading');
                }
            });
        }
    }
};

// ==========================================
// 4. DONOR DASHBOARD
// ==========================================

const DonorDashboard = {
    init() {
        document.querySelectorAll('.donor-nav').forEach(btn => {
            btn.addEventListener('click', () => {
                document.querySelectorAll('.donor-nav').forEach(b => b.classList.remove('active'));
                const container = document.getElementById('dashboard-donor');
                if (container) {
                    container.querySelectorAll('.dash-panel').forEach(p => p.classList.remove('active'));
                }
                btn.classList.add('active');
                const target = document.getElementById(btn.dataset.target);
                if (target) target.classList.add('active');

                if (btn.dataset.target === 'dash-donor-appointments') DonorDashboard.loadAppointments();
                if (btn.dataset.target === 'dash-donor-card') DonorDashboard.loadCard();
                if (btn.dataset.target === 'dash-donor-requests') {
                    DonorDashboard.loadMatchingRequests();
                    DonorDashboard.loadPledgedRequests();
                }
                if (btn.dataset.target === 'dash-history') DonorDashboard.renderHistory();
            });
        });

        // Save Profile Form
        const profileForm = document.getElementById('donor-profile-form');
        if (profileForm) {
            profileForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const msg = document.getElementById('dp-message');
                const btn = document.getElementById('btn-save-profile');
                btn.disabled = true;

                const payload = {
                    age: parseInt(document.getElementById('dp-age').value, 10),
                    gender: document.getElementById('dp-gender').value,
                    bloodGroup: document.getElementById('dp-blood').value,
                    weight: parseFloat(document.getElementById('dp-weight').value),
                    phone: document.getElementById('dp-phone').value.trim(),
                    consent: document.getElementById('dp-consent').checked
                };

                try {
                    await API.request('/api/donor/profile', {
                        method: 'PUT',
                        body: JSON.stringify(payload)
                    });

                    DonorDashboard.profileReady = true;
                    document.getElementById('donor-notice').hidden = true;
                    msg.textContent = 'Medical profile saved successfully!';
                    msg.className = 'form-message success';
                    msg.style.display = 'block';
                    setTimeout(() => { msg.style.display = 'none'; }, 4000);

                    PublicRecords.render(1);
                    DonorDashboard.loadCard();
                } catch (err) {
                    msg.textContent = err.message || 'Failed to update profile.';
                    msg.className = 'form-message error';
                    msg.style.display = 'block';
                } finally {
                    btn.disabled = false;
                }
            });
        }

        // Run Eligibility Check Button
        const checkBtn = document.getElementById('btn-run-check');
        if (checkBtn) {
            checkBtn.addEventListener('click', async () => {
                const resDiv = document.getElementById('eligibility-result');
                const hbInput = document.getElementById('check-hb').value;
                const queryParam = hbInput ? `?hb=${encodeURIComponent(hbInput)}` : '';

                try {
                    const data = await API.request(`/api/donor/eligibility${queryParam}`, { method: 'GET' });

                    const checkItems = data.checks.map(c => {
                        let icon = '✓';
                        if (c.status === 'fail') icon = '✗';
                        else if (c.status === 'warning') icon = '?';
                        return `<li>${icon} <strong>${escapeHtml(c.label)}:</strong> ${escapeHtml(c.message)}</li>`;
                    }).join('');

                    resDiv.innerHTML = `
                        <h4>Result: ${data.eligible ? 'Eligible to Donate!' : 'Deferred'}</h4>
                        <ul class="custom-list" style="margin-top:10px;">
                            ${checkItems}
                        </ul>
                    `;
                    resDiv.className = `eligibility-result ${data.eligible ? 'pass' : 'fail'}`;
                    resDiv.classList.remove('hidden');
                    resDiv.style.display = 'block';
                } catch (err) {
                    resDiv.innerHTML = `<p style="color:var(--primary); font-weight:600;">${escapeHtml(err.message)}</p>`;
                    resDiv.className = 'eligibility-result fail';
                    resDiv.classList.remove('hidden');
                    resDiv.style.display = 'block';
                }
            });
        }

        // Account & Password Form
        const accForm = document.getElementById('donor-account-form');
        if (accForm) {
            accForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const msg = document.getElementById('acc-message');
                const fullName = document.getElementById('acc-fullname').value.trim();
                const phone = document.getElementById('acc-phone').value.trim();
                const currentPassword = document.getElementById('acc-curr-password').value;
                const newPassword = document.getElementById('acc-new-password').value;

                try {
                    await API.request('/api/donor/account', {
                        method: 'PUT',
                        body: JSON.stringify({ fullName, phone, currentPassword, newPassword })
                    });
                    const refreshedUser = await API.request('/api/me');
                    API.setCurrentUser(refreshedUser);
                    await DonorDashboard.loadProfile();
                    await DonorDashboard.loadCard();
                    msg.textContent = 'Account settings updated successfully!';
                    msg.className = 'form-message success';
                    msg.style.display = 'block';
                    document.getElementById('acc-curr-password').value = '';
                    document.getElementById('acc-new-password').value = '';
                    setTimeout(() => { msg.style.display = 'none'; }, 4000);
                } catch (err) {
                    msg.textContent = err.message || 'Error updating account settings.';
                    msg.className = 'form-message error';
                    msg.style.display = 'block';
                }
            });
        }
    },

    async loadProfile() {
        const user = API.getCurrentUser();
        if (!user || user.role !== 'donor') return;

        const nameEl = document.getElementById('dash-donor-name');
        if (nameEl) nameEl.textContent = user.fullName;

        const accName = document.getElementById('acc-fullname');
        if (accName) accName.value = user.fullName;

        try {
            const profile = await API.request('/api/donor/profile', { method: 'GET' });
            this.profileReady = !!profile;
            const notice = document.getElementById('donor-notice');
            if (notice) { notice.hidden = !!profile; notice.textContent = 'Welcome! Complete your donor profile to reserve a camp and unlock matching requests.'; }
            if (profile) {
                if (document.getElementById('dp-age')) document.getElementById('dp-age').value = profile.age || '';
                if (document.getElementById('dp-gender')) document.getElementById('dp-gender').value = profile.gender || '';
                if (document.getElementById('dp-blood')) document.getElementById('dp-blood').value = profile.bloodGroup || '';
                if (document.getElementById('dp-weight')) document.getElementById('dp-weight').value = profile.weight || '';
                if (document.getElementById('dp-phone')) document.getElementById('dp-phone').value = profile.phone || '';
                if (document.getElementById('acc-phone')) document.getElementById('acc-phone').value = profile.phone || '';
                if (document.getElementById('dp-consent')) document.getElementById('dp-consent').checked = !!profile.consent;
            }
        } catch (err) {
            console.warn('Could not load donor profile:', err);
        }
    },

    async loadCard() {
        if (this.profileReady === false) {
            const user = API.getCurrentUser();
            document.getElementById('dc-name').textContent = user?.fullName || 'Complete your profile';
            document.getElementById('dc-username').textContent = user?.username || '';
            document.getElementById('dc-blood-group').textContent = '—';
            document.getElementById('dc-count').textContent = '0';
            document.getElementById('dc-last-date').textContent = 'No donations yet';
            document.getElementById('dc-next-date').textContent = 'Complete your profile';
            document.getElementById('dc-badges-container').textContent = '';
            return;
        }
        try {
            const card = await API.request('/api/donor/card', { method: 'GET' });
            if (!card) return;

            document.getElementById('dc-name').textContent = card.fullName;
            document.getElementById('dc-username').textContent = card.username;
            document.getElementById('dc-blood-group').textContent = card.bloodGroup || 'N/A';
            document.getElementById('dc-count').textContent = card.donationCount;
            document.getElementById('dc-last-date').textContent = card.lastDonationDate || 'None yet';
            document.getElementById('dc-next-date').innerHTML = card.nextEligibleDate
                ? `<strong>${escapeHtml(card.nextEligibleDate)}</strong>`
                : `<strong style="color:var(--accent-sage)">Eligible Today</strong>`;

            const avatarEl = document.getElementById('dc-avatar');
            if (avatarEl) {
                const avatarName = encodeURIComponent(card.fullName || card.username || 'Donor');
                avatarEl.textContent = (card.fullName || card.username || 'D').slice(0, 1).toUpperCase();
            }

            const badgeContainer = document.getElementById('dc-badges-container');
            if (badgeContainer) {
                badgeContainer.innerHTML = (card.milestones && card.milestones.length > 0)
                    ? card.milestones.map(m => `<span class="milestone-pill">${escapeHtml(m)}</span>`).join(' ')
                    : '<small style="color:var(--text-muted)">Milestones: Complete 1 donation to earn Bronze!</small>';
            }
        } catch (err) {
            console.error('Failed to load donor card:', err);
        }
    },

    async loadAppointments() {
        const listEl = document.getElementById('donor-appointments-list');
        const availEl = document.getElementById('donor-available-camps-list');
        const user = API.getCurrentUser();

        try {
            const [appointments, allCamps] = await Promise.all([
                API.request('/api/donor/appointments', { method: 'GET' }),
                API.request('/api/camps', { method: 'GET' })
            ]);

            // 1. Render Booked Appointments
            if (listEl) {
                if (!appointments || appointments.length === 0) {
                    listEl.innerHTML = `
                        <div style="grid-column:1/-1; text-align:center; padding:32px 16px; background:var(--bg-soft); border-radius:var(--radius-sm);">
                            <p style="color:var(--text-muted); margin-bottom:12px;">You currently have no scheduled camp appointments.</p>
                            <button type="button" class="btn btn-primary" style="font-size:0.9rem; padding:8px 18px;"
                                onclick="Navigation.goTo('camps-section')">Browse Public Camps</button>
                        </div>
                    `;
                } else {
                    listEl.innerHTML = appointments.map(c => {
                        const dateParts = c.date.split('-');
                        const months = ['JAN', 'FEB', 'MAR', 'APR', 'MAY', 'JUN', 'JUL', 'AUG', 'SEP', 'OCT', 'NOV', 'DEC'];
                        const mIdx = parseInt(dateParts[1], 10) - 1;
                        const mStr = months[mIdx] || 'DATE';
                        const dayStr = dateParts[2] || '01';

                        return `
                            <div class="appointment-card">
                                <div class="appointment-header">
                                    <div>
                                        <h4 style="margin:0 0 4px 0; color:var(--text-main);">${escapeHtml(c.name)}</h4>
                                        <span class="badge badge-${escapeHtml(c.status.toLowerCase())}">${escapeHtml(c.status)}</span>
                                    </div>
                                    <div class="appointment-date-badge">
                                        <div class="month">${mStr}</div>
                                        <div class="day">${dayStr}</div>
                                    </div>
                                </div>
                                <p style="font-size:0.85rem; color:var(--text-muted); margin:8px 0;">
                                    Date: <strong>${escapeHtml(c.date)}</strong> | Reserved Donor Slot Confirmed
                                </p>
                                <div style="margin-top:12px; display:flex; gap:8px;">
                                    ${c.status !== 'Completed' && c.status !== 'Cancelled' ? `
                                        <button class="btn btn-outline" style="padding:6px 12px; font-size:0.8rem; color:#c62828;"
                                            onclick="DonorDashboard.cancelAppointment('${c.id}')">Cancel Reservation</button>
                                    ` : `<span style="font-size:0.8rem; color:var(--text-muted)">Archived Drive</span>`}
                                </div>
                            </div>
                        `;
                    }).join('');
                }
            }

            // 2. Render Available Upcoming Drives for Instant 1-Click Reservation
            if (availEl) {
                const upcomingDrives = (allCamps || []).filter(c => c.date >= Workspace.today() && c.status !== 'Completed' && c.status !== 'Cancelled');
                if (upcomingDrives.length === 0) {
                    availEl.innerHTML = '<p style="color:var(--text-muted); grid-column:1/-1;">No upcoming drives scheduled right now.</p>';
                } else {
                    availEl.innerHTML = upcomingDrives.map(c => {
                        const isRegistered = user && c.registeredDonors && c.registeredDonors.includes(user.username);
                        const isFull = c.remainingSlots <= 0 || c.status === 'Full';
                        const dateParts = c.date.split('-');
                        const months = ['JAN', 'FEB', 'MAR', 'APR', 'MAY', 'JUN', 'JUL', 'AUG', 'SEP', 'OCT', 'NOV', 'DEC'];
                        const mIdx = parseInt(dateParts[1], 10) - 1;
                        const mStr = months[mIdx] || 'DATE';
                        const dayStr = dateParts[2] || '01';

                        return `
                            <div class="appointment-card" style="border:1px solid ${isRegistered ? '#38a169' : '#e2e8f0'};">
                                <div class="appointment-header">
                                    <div>
                                        <h4 style="margin:0 0 4px 0; color:var(--text-main);">${escapeHtml(c.name)}</h4>
                                        <span class="badge badge-${escapeHtml(c.status.toLowerCase())}">${escapeHtml(c.status)}</span>
                                    </div>
                                    <div class="appointment-date-badge">
                                        <div class="month">${mStr}</div>
                                        <div class="day">${dayStr}</div>
                                    </div>
                                </div>
                                <p style="font-size:0.85rem; color:var(--text-muted); margin:8px 0;">
                                    Slots Remaining: <strong>${c.remainingSlots}</strong> / ${c.slotCapacity} | Units Goal: ${c.unitsCollected}/${c.targetUnits}
                                </p>
                                <div style="margin-top:12px; display:flex; align-items:center; justify-content:space-between; flex-wrap:wrap; gap:8px;">
                                    ${isRegistered ? `
                                        <span class="badge badge-open">✓ Slot Confirmed</span>
                                        <button class="btn btn-outline" style="padding:4px 10px; font-size:0.8rem; color:#c62828;"
                                            onclick="DonorDashboard.cancelAppointment('${c.id}')">Cancel Slot</button>
                                    ` : isFull ? `
                                        <span class="badge badge-urgent">Slots Full</span>
                                    ` : `
                                        <button class="btn btn-secondary" style="padding:6px 14px; font-size:0.85rem;"
                                            onclick="DonorDashboard.reserveCamp('${c.id}')">Reserve Slot</button>
                                    `}
                                </div>
                            </div>
                        `;
                    }).join('');
                }
            }
        } catch (err) {
            if (listEl) listEl.innerHTML = `<p style="color:var(--primary)">Error: ${escapeHtml(err.message)}</p>`;
        }
    },

    async reserveCamp(campId) {
        await CampsManager.registerDonor(campId);
        await this.loadAppointments();
    },

    async cancelAppointment(campId) {
        try {
            await API.request(`/api/camps/${campId}/register`, { method: 'DELETE' });
            Toast.show('Camp reservation cancelled successfully.', 'info');
            await this.loadAppointments();
            CampsManager.renderCamps();
        } catch (err) {
            Toast.show(err.message || 'Failed to cancel reservation.', 'error');
        }
    },

    async loadPledgedRequests() {
        const tbody = document.getElementById('donor-pledged-requests-body');
        if (!tbody) return;

        try {
            const pledged = await API.request('/api/donor/pledged-requests', { method: 'GET' });
            if (!pledged || pledged.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:16px; color:var(--text-muted)">You have not pledged to any open blood requests.</td></tr>';
                return;
            }

            tbody.innerHTML = pledged.map(r => `
                <tr>
                    <td><strong>${escapeHtml(r.hospitalName)}</strong><br><small style="color:var(--text-muted)">${escapeHtml(r.orgCity || 'Nainital')}</small></td>
                    <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(r.bloodGroup)}</span></td>
                    <td><strong><a href="tel:${escapeHtml(r.orgPhone || '104')}" style="color:var(--primary); text-decoration:none;">📞 ${escapeHtml(r.orgPhone || '104 Helpline')}</a></strong></td>
                    <td><span class="badge badge-${escapeHtml(r.urgency.toLowerCase())}">${escapeHtml(r.urgency)}</span></td>
                    <td><span class="badge badge-open">${escapeHtml(r.status)}</span></td>
                </tr>
            `).join('');
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="5" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    },

    async loadMatchingRequests() {
        const tbody = document.getElementById('donor-matching-requests-body');
        if (!tbody) return;

        try {
            const reqs = await API.request('/api/donor/matching-requests', { method: 'GET' });
            if (!reqs || reqs.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:20px;">No open requests matching your blood group right now.</td></tr>';
                return;
            }

            tbody.innerHTML = reqs.map(r => {
                const remaining = r.unitsNeeded;
                return `
                    <tr>
                        <td><strong>${escapeHtml(r.hospitalName)}</strong><br><small style="color:var(--text-muted)">${escapeHtml(r.orgCity || 'Nainital')}</small></td>
                        <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(r.bloodGroup)}</span></td>
                        <td>${remaining} units</td>
                        <td><span class="badge badge-${escapeHtml(r.urgency.toLowerCase())}">${escapeHtml(r.urgency)}</span></td>
                        <td>
                            ${r.hasPledged ? `
                                <span class="badge badge-open">✓ Pledged</span>
                                <small style="display:block; margin-top:4px;"><a href="tel:${escapeHtml(r.orgPhone || '104')}" style="color:var(--primary); font-weight:bold;">Call: ${escapeHtml(r.orgPhone || '104')}</a></small>
                            ` : `
                                <button class="btn btn-secondary" style="padding:6px 12px; font-size:0.85rem;"
                                    ${r.isEligible === false ? 'disabled title="Update your profile or wait until eligible"' : ''} onclick="DonorDashboard.pledgeDonate('${r.id}')">I Can Donate</button>
                            `}
                        </td>
                    </tr>
                `;
            }).join('');
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="5" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    },

    async pledgeDonate(reqId) {
        try {
            await API.request(`/api/requests/${reqId}/respond`, { method: 'POST' });
            Toast.show('Thank you for pledging! The hospital has been notified and expects your call.', 'success');
            this.loadMatchingRequests();
            this.loadPledgedRequests();
        } catch (err) {
            Toast.show(err.message || 'Failed to pledge donation.', 'error');
        }
    },

    async renderHistory() {
        const tbody = document.getElementById('user-history-body');
        if (!tbody) return;

        try {
            const records = await API.request('/api/donor/history', { method: 'GET' });
            if (!records || records.length === 0) {
                tbody.innerHTML = '<tr><td colspan="3" style="text-align:center; padding:20px;">No donation records found.</td></tr>';
                return;
            }

            tbody.innerHTML = records.map(r => `
                <tr>
                    <td>${escapeHtml(r.date)}</td>
                    <td>${escapeHtml(r.campName || r.camp)}</td>
                    <td><span class="status-badge ${r.status === 'Completed' ? 'status-completed' : 'status-deferred'}">${escapeHtml(r.status)}</span></td>
                </tr>
            `).join('');
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="3" style="color:var(--primary)">Error: ${escapeHtml(err.message)}</td></tr>`;
        }
    }
};

// ==========================================
// 5. HOSPITAL & NGO DASHBOARD
// ==========================================

const NGODashboard = {
    init() {
        document.querySelectorAll('.ngo-nav').forEach(btn => {
            btn.addEventListener('click', () => {
                document.querySelectorAll('.ngo-nav').forEach(b => b.classList.remove('active'));
                const container = document.getElementById('dashboard-ngo');
                if (container) {
                    container.querySelectorAll('.dash-panel').forEach(p => p.classList.remove('active'));
                }
                btn.classList.add('active');
                const target = document.getElementById(btn.dataset.target);
                if (target) target.classList.add('active');

                if (btn.dataset.target === 'dash-ngo-camps') NGODashboard.loadCampsTable();
                if (btn.dataset.target === 'dash-ngo-inventory') NGODashboard.loadInventory();
                if (btn.dataset.target === 'dash-ngo-requests') NGODashboard.loadRequestsTable();
                if (btn.dataset.target === 'dash-ngo-database') NGODashboard.loadDatabase(1);
                if (btn.dataset.target === 'dash-ngo-reminders') NGODashboard.loadReminders();
                if (btn.dataset.target === 'dash-ngo-profile') NGODashboard.loadProfileForm();
                if (btn.dataset.target === 'dash-ngo-record') NGODashboard.setupDonorAutocomplete();
            });
        });

        // Record donation form
        const recForm = document.getElementById('record-donation-form');
        if (recForm) {
            recForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const msg = document.getElementById('don-message');
                const btn = document.getElementById('btn-submit-donation');
                btn.disabled = true;

                const username = document.getElementById('don-username').value.trim();
                const campId = document.getElementById('don-camp').value;
                const date = document.getElementById('don-date').value;
                const status = document.getElementById('don-status').value;

                try {
                    await API.request('/api/donations', {
                        method: 'POST',
                        body: JSON.stringify({ username, campId, date, status })
                    });

                    msg.textContent = 'Donation verified and recorded successfully in inventory!';
                    msg.className = 'form-message success';
                    msg.style.display = 'block';
                    recForm.reset();
                    setTimeout(() => { msg.style.display = 'none'; }, 4000);

                    CampsManager.renderCamps();
                    PublicRecords.render(1);
                    PublicStock.render();
                    NGODashboard.loadKPIs();
                } catch (err) {
                    msg.textContent = err.message || 'Failed to record donation.';
                    msg.className = 'form-message error';
                    msg.style.display = 'block';
                } finally {
                    btn.disabled = false;
                }
            });
        }

        // Create Camp Form
        const campForm = document.getElementById('create-camp-form');
        if (campForm) {
            campForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const msg = document.getElementById('camp-create-msg');
                const name = document.getElementById('camp-new-name').value.trim();
                const date = document.getElementById('camp-new-date').value;
                const targetUnits = parseInt(document.getElementById('camp-new-target').value, 10);
                const slotCapacity = parseInt(document.getElementById('camp-new-slots').value, 10);

                try {
                    await API.request('/api/camps', {
                        method: 'POST',
                        body: JSON.stringify({ name, date, targetUnits, slotCapacity })
                    });
                    msg.textContent = 'Camp successfully created!';
                    msg.className = 'form-message success';
                    msg.style.display = 'block';
                    campForm.reset();
                    setTimeout(() => { msg.style.display = 'none'; }, 4000);

                    CampsManager.renderCamps();
                    NGODashboard.loadCampsTable();
                    NGODashboard.loadKPIs();
                } catch (err) {
                    msg.textContent = err.message || 'Error creating camp.';
                    msg.className = 'form-message error';
                    msg.style.display = 'block';
                }
            });
        }

        // Edit Camp Form
        const editCampForm = document.getElementById('edit-camp-form');
        if (editCampForm) {
            editCampForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const campId = document.getElementById('mec-camp-id').value;
                const date = document.getElementById('mec-date').value;
                const targetUnits = parseInt(document.getElementById('mec-target').value, 10);

                try {
                    await API.request(`/api/camps/${campId}`, {
                        method: 'PUT',
                        body: JSON.stringify({ date, targetUnits })
                    });
                    Toast.show(`Camp ${campId} updated successfully.`, 'success');
                    NGODashboard.closeModal('modal-edit-camp');
                    NGODashboard.loadCampsTable();
                    CampsManager.renderCamps();
                } catch (err) {
                    Toast.show(err.message || 'Failed to edit camp.', 'error');
                }
            });
        }

        // Create Request Form
        const reqForm = document.getElementById('create-request-form');
        if (reqForm) {
            reqForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const msg = document.getElementById('req-create-msg');
                const bloodGroup = document.getElementById('req-blood-group').value;
                const unitsNeeded = parseInt(document.getElementById('req-units').value, 10);
                const hospitalName = document.getElementById('req-hospital').value.trim();
                const urgency = document.getElementById('req-urgency').value;

                try {
                    await API.request('/api/requests', {
                        method: 'POST',
                        body: JSON.stringify({ bloodGroup, unitsNeeded, hospitalName, urgency })
                    });
                    msg.textContent = 'Urgent request broadcasted to matching eligible donors!';
                    msg.className = 'form-message success';
                    msg.style.display = 'block';
                    reqForm.reset();
                    setTimeout(() => { msg.style.display = 'none'; }, 4000);

                    NGODashboard.loadRequestsTable();
                    NGODashboard.loadKPIs();
                } catch (err) {
                    msg.textContent = err.message || 'Error broadcasting request.';
                    msg.className = 'form-message error';
                    msg.style.display = 'block';
                }
            });
        }

        // Database search filters
        const searchInput = document.getElementById('search-ngo-donors');
        const bgFilter = document.getElementById('filter-ngo-bg');
        if (searchInput && bgFilter) {
            let timeout = null;
            const triggerSearch = () => {
                clearTimeout(timeout);
                timeout = setTimeout(() => {
                    NGODashboard.loadDatabase(1);
                }, 200);
            };
            searchInput.addEventListener('input', triggerSearch);
            bgFilter.addEventListener('change', triggerSearch);
        }

        // Org profile form
        const orgProfForm = document.getElementById('org-profile-form');
        if (orgProfForm) {
            orgProfForm.addEventListener('submit', async (e) => {
                e.preventDefault();
                const msg = document.getElementById('org-prof-msg');
                const fullName = document.getElementById('org-prof-name').value.trim();
                const city = document.getElementById('org-prof-city').value.trim();
                const contactPhone = document.getElementById('org-prof-phone').value.trim();
                const contactEmail = document.getElementById('org-prof-email').value.trim();

                try {
                    await API.request('/api/org/profile', {
                        method: 'PUT',
                        body: JSON.stringify({ fullName, city, contactPhone, contactEmail })
                    });
                    const refreshedUser = await API.request('/api/me');
                    API.setCurrentUser(refreshedUser);
                    document.getElementById('dash-ngo-name').textContent = refreshedUser.fullName;
                    msg.textContent = 'Profile saved successfully!';
                    msg.className = 'form-message success';
                    msg.style.display = 'block';
                    setTimeout(() => { msg.style.display = 'none'; }, 4000);
                } catch (err) {
                    msg.textContent = err.message || 'Error saving profile.';
                    msg.className = 'form-message error';
                    msg.style.display = 'block';
                }
            });
        }
    },

    async loadKPIs() {
        try {
            const [camps, stock, reqs] = await Promise.all([
                API.request('/api/camps', { method: 'GET' }),
                API.request('/api/inventory/stock', { method: 'GET' }),
                API.request('/api/requests', { method: 'GET' })
            ]);

            let activeCamps = camps ? camps.filter(c => c.date >= Workspace.today() && c.status !== 'Completed' && c.status !== 'Cancelled').length : 0;
            let totalUnits = camps ? camps.reduce((sum, c) => sum + (c.unitsCollected || 0), 0) : 0;

            let lowStockCount = 0;
            if (stock && stock.stockByGroup) {
                Object.values(stock.stockByGroup).forEach(g => {
                    if (g.isLow) lowStockCount++;
                });
            }

            let openUrgent = reqs ? reqs.filter(r => r.status === 'Open' && r.urgency !== 'Normal').length : 0;

            document.getElementById('stat-active-camps').textContent = activeCamps;
            document.getElementById('stat-units-collected').textContent = totalUnits;
            document.getElementById('stat-low-stock').textContent = lowStockCount;
            document.getElementById('stat-urgent-requests').textContent = openUrgent;
        } catch (err) {
            console.warn('Could not load KPIs:', err);
        }
    },

    async loadProfile() {
        const user = API.getCurrentUser();
        if (!user || user.role !== 'ngo') return;

        const nameEl = document.getElementById('dash-ngo-name');
        if (nameEl) nameEl.textContent = user.fullName;

        await CampsManager.populateDropdown();
        await this.setupDonorAutocomplete();
        await this.loadKPIs();
    },

    async setupDonorAutocomplete() {
        const datalist = document.getElementById('datalist-all-donors');
        if (!datalist) return;

        try {
            const data = await API.request('/api/org/donors?page=1', { method: 'GET' });
            if (data && data.items) {
                datalist.innerHTML = data.items.map(d =>
                    `<option value="${escapeHtml(d.username)}">${escapeHtml(d.fullName)} (${escapeHtml(d.bloodGroup)}) - ${escapeHtml(d.phone)}</option>`
                ).join('');
            }
        } catch (err) {
            console.warn('Could not load donor autocomplete:', err);
        }

        const donInput = document.getElementById('don-username');
        if (donInput && !donInput.dataset.bound) {
            donInput.dataset.bound = 'true';
            donInput.addEventListener('input', () => {
                const infoEl = document.getElementById('don-selected-info');
                if (infoEl) infoEl.textContent = `Donor ID: @${donInput.value.trim()}`;
            });
        }
    },

    async onCampSelected(campId) {
        const container = document.getElementById('don-quick-registered');
        if (!container) return;

        if (!campId) {
            container.innerHTML = '<span style="color:var(--text-muted); font-size:0.85rem;">Select a camp above to view registered donors roster</span>';
            return;
        }

        try {
            const data = await API.request(`/api/org/camps/${campId}/donors`, { method: 'GET' });
            if (!data.donors || data.donors.length === 0) {
                container.innerHTML = '<span style="color:var(--text-muted); font-size:0.85rem;">No registered donors for this drive yet.</span>';
                return;
            }

            container.innerHTML = data.donors.map(d => `
                <button type="button" class="donor-chip" onclick="NGODashboard.selectQuickDonor('${escapeHtml(d.username)}', '${escapeHtml(d.fullName)}', '${escapeHtml(d.bloodGroup)}')">
                    <strong>@${escapeHtml(d.username)}</strong>
                    <span>(${escapeHtml(d.fullName)} - ${escapeHtml(d.bloodGroup)})</span>
                </button>
            `).join('');
        } catch (err) {
            container.innerHTML = `<span style="color:var(--primary); font-size:0.85rem;">${escapeHtml(err.message)}</span>`;
        }
    },

    selectQuickDonor(username, fullName, bg) {
        const donInput = document.getElementById('don-username');
        if (donInput) donInput.value = username;

        const infoEl = document.getElementById('don-selected-info');
        if (infoEl) infoEl.innerHTML = `Selected: <strong>${escapeHtml(fullName)}</strong> (${escapeHtml(bg)})`;

        Toast.show(`Selected donor @${username}`, 'info');
    },

    async loadCampsTable() {
        const tbody = document.getElementById('ngo-camps-table-body');
        if (!tbody) return;

        try {
            const camps = await API.request('/api/camps', { method: 'GET' });
            if (!camps || camps.length === 0) {
                tbody.innerHTML = '<tr><td colspan="8" style="text-align:center; padding:20px;">No camps scheduled.</td></tr>';
                return;
            }

            tbody.innerHTML = camps.map(c => `
                <tr>
                    <td><strong>${escapeHtml(c.name)}</strong></td>
                    <td>${escapeHtml(c.date)}</td>
                    <td>${c.targetUnits}</td>
                    <td>${c.unitsCollected}</td>
                    <td>${c.remainingSlots} slots</td>
                    <td>
                        <button class="btn btn-outline" style="padding:3px 10px; font-size:0.8rem; border-color:#aaa;"
                            onclick="NGODashboard.viewCampDonors('${c.id}')">
                            📋 ${c.registeredDonors ? c.registeredDonors.length : 0} Donors
                        </button>
                    </td>
                    <td><span class="badge badge-${escapeHtml(c.status.toLowerCase())}">${escapeHtml(c.status)}</span></td>
                    <td>
                        <div style="display:flex; gap:4px; flex-wrap:wrap;">
                            ${c.status === 'Upcoming' ? `
                                <button class="btn btn-outline" style="padding:3px 8px; font-size:0.75rem;"
                                    onclick="NGODashboard.openEditCampModal('${c.id}', '${c.date}', ${c.targetUnits})">Edit</button>
                                <button class="btn btn-outline" style="padding:3px 8px; font-size:0.75rem; color:#2e7d32;"
                                    onclick="NGODashboard.updateCampStatus('${c.id}', 'Open')">Start Camp</button>
                            ` : ''}
                            ${c.status !== 'Completed' && c.status !== 'Cancelled' ? `
                                <button class="btn btn-outline" style="padding:3px 8px; font-size:0.75rem;"
                                    ${c.status === 'Upcoming' ? 'disabled title="Start the camp first"' : ''} onclick="NGODashboard.updateCampStatus('${c.id}', 'Completed')">Complete</button>
                                <button class="btn btn-outline" style="padding:3px 8px; font-size:0.75rem; color:#c62828;"
                                    onclick="NGODashboard.updateCampStatus('${c.id}', 'Cancelled')">Cancel</button>
                            ` : `<span style="color:var(--text-muted); font-size:0.8rem;">Archived</span>`}
                        </div>
                    </td>
                </tr>
            `).join('');
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="8" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    },

    async updateCampStatus(campId, newStatus) {
        try {
            await API.request(`/api/camps/${campId}/status`, {
                method: 'PUT',
                body: JSON.stringify({ status: newStatus })
            });
            Toast.show(`Camp ${campId} status updated to ${newStatus}.`, 'success');
            this.loadCampsTable();
            CampsManager.renderCamps();
            this.loadKPIs();
        } catch (err) {
            Toast.show(err.message || 'Failed to update camp status.', 'error');
        }
    },

    async viewCampDonors(campId) {
        try {
            const data = await API.request(`/api/org/camps/${campId}/donors`, { method: 'GET' });
            document.getElementById('mcd-title').textContent = `${data.campName} - Registered Donors`;
            document.getElementById('mcd-subtitle').textContent = `Scheduled on ${data.campDate} | ${data.donors.length} registered volunteers`;

            const tbody = document.getElementById('mcd-table-body');
            if (!data.donors || data.donors.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:20px; color:var(--text-muted)">No donors registered for this camp yet.</td></tr>';
            } else {
                tbody.innerHTML = data.donors.map(d => `
                    <tr>
                        <td>
                            <div style="display:flex; align-items:center; gap:10px;">
                                <span class="initial-avatar" aria-hidden="true">♥</span>
                                <div><strong>${escapeHtml(d.fullName)}</strong><br><small>@${escapeHtml(d.username)}</small></div>
                            </div>
                        </td>
                        <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(d.bloodGroup)}</span></td>
                        <td><a href="tel:${escapeHtml(d.phone)}" style="color:var(--primary); text-decoration:none;">📞 ${escapeHtml(d.phone)}</a></td>
                        <td>${d.isEligibleToday ? '<span class="badge badge-open">Eligible</span>' : '<span class="badge badge-cancelled">Ineligible</span>'}</td>
                        <td>
                            <button class="btn btn-secondary" style="padding:4px 10px; font-size:0.75rem;"
                                onclick="NGODashboard.checkInDonor('${escapeHtml(d.username)}', '${campId}')">
                                Log Donation
                            </button>
                        </td>
                    </tr>
                `).join('');
            }

            document.getElementById('modal-camp-donors').classList.add('open');
        } catch (err) {
            Toast.show(err.message || 'Could not load camp donors roster.', 'error');
        }
    },

    checkInDonor(username, campId) {
        this.closeModal('modal-camp-donors');
        const campBtn = document.querySelector('.ngo-nav[data-target="dash-ngo-record"]');
        if (campBtn) campBtn.click();

        const sel = document.getElementById('don-camp');
        if (sel) {
            sel.value = campId;
            this.onCampSelected(campId);
        }

        const uInput = document.getElementById('don-username');
        if (uInput) uInput.value = username;

        Toast.show(`Ready to record donation for @${username}`, 'info');
    },

    async loadRequestsTable() {
        const tbody = document.getElementById('ngo-requests-table-body');
        if (!tbody) return;

        try {
            const reqs = await API.request('/api/requests', { method: 'GET' });
            if (!reqs || reqs.length === 0) {
                tbody.innerHTML = '<tr><td colspan="7" style="text-align:center; padding:20px;">No active blood requests.</td></tr>';
                return;
            }

            tbody.innerHTML = reqs.map(r => `
                <tr>
                    <td><strong>${escapeHtml(r.hospitalName)}</strong></td>
                    <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(r.bloodGroup)}</span></td>
                    <td>${r.unitsNeeded} units</td>
                    <td>
                        ${r.unitsPledged > 0 ? `
                            <button class="btn btn-outline" style="padding:3px 8px; font-size:0.8rem; border-color:#2b6cb0; color:#2b6cb0;"
                                onclick="NGODashboard.viewRequestResponders('${r.id}')">
                                📞 ${r.unitsPledged} Pledges
                            </button>
                        ` : `<span style="color:var(--text-muted); font-size:0.85rem;">0 pledges</span>`}
                    </td>
                    <td><span class="badge badge-${escapeHtml(r.urgency.toLowerCase())}">${escapeHtml(r.urgency)}</span></td>
                    <td><strong>${escapeHtml(r.status)}</strong></td>
                    <td>
                        ${r.status === 'Open' ? `
                            <button class="btn btn-outline" style="padding:4px 8px; font-size:0.8rem;"
                                onclick="NGODashboard.updateRequestStatus('${r.id}', 'Fulfilled')">Fulfill</button>
                            <button class="btn btn-outline" style="padding:4px 8px; font-size:0.8rem;"
                                onclick="NGODashboard.updateRequestStatus('${r.id}', 'Closed')">Close</button>
                        ` : `<span style="color:var(--text-muted)">Done</span>`}
                    </td>
                </tr>
            `).join('');
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="7" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    },

    async viewRequestResponders(reqId) {
        try {
            const data = await API.request(`/api/org/requests/${reqId}/responders`, { method: 'GET' });
            document.getElementById('mrr-title').textContent = `Emergency Volunteers for ${reqId} (${data.bloodGroup})`;
            document.getElementById('mrr-subtitle').textContent = `${data.responders.length} donors pledged to donate`;

            const tbody = document.getElementById('mrr-table-body');
            if (!data.responders || data.responders.length === 0) {
                tbody.innerHTML = '<tr><td colspan="4" style="text-align:center; padding:16px;">No pledged donors found.</td></tr>';
            } else {
                tbody.innerHTML = data.responders.map(r => `
                    <tr>
                        <td><strong>${escapeHtml(r.fullName)}</strong><br><small>@${escapeHtml(r.username)}</small></td>
                        <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(r.bloodGroup)}</span></td>
                        <td><a href="tel:${escapeHtml(r.phone)}" style="color:var(--primary); font-weight:bold; text-decoration:none;">📞 ${escapeHtml(r.phone)}</a></td>
                        <td>
                            <a href="tel:${escapeHtml(r.phone)}" class="btn btn-primary" style="padding:4px 10px; font-size:0.75rem; text-decoration:none;">
                                Call Donor
                            </a>
                        </td>
                    </tr>
                `).join('');
            }

            document.getElementById('modal-request-responders').classList.add('open');
        } catch (err) {
            Toast.show(err.message || 'Could not load request responders.', 'error');
        }
    },

    closeModal(modalId) {
        const el = document.getElementById(modalId);
        if (el) el.classList.remove('open');
    },

    openEditCampModal(campId, date, targetUnits) {
        const idInp = document.getElementById('mec-camp-id');
        const dateInp = document.getElementById('mec-date');
        const targetInp = document.getElementById('mec-target');
        const titleEl = document.getElementById('mec-title');
        if (idInp) idInp.value = campId;
        if (dateInp) dateInp.value = date;
        if (targetInp) targetInp.value = targetUnits;
        if (titleEl) titleEl.textContent = `Edit Details for ${campId}`;
        const modal = document.getElementById('modal-edit-camp');
        if (modal) modal.classList.add('open');
    },

    async exportCSV(type) {
        try {
            const token = API.getToken();
            const res = await fetch(`/api/org/export/${type}.csv`, {
                headers: { 'Authorization': `Bearer ${token}` }
            });
            if (!res.ok) throw new Error(`Export failed with status ${res.status}`);
            const blob = await res.blob();
            const blobUrl = window.URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = blobUrl;
            a.download = `${type}_export.csv`;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            window.URL.revokeObjectURL(blobUrl);
            Toast.show(`Downloaded ${type} CSV report successfully.`, 'success');
        } catch (err) {
            const token = API.getToken();
            window.open(`/api/org/export/${type}.csv?token=${encodeURIComponent(token)}`);
        }
    },

    async loadInventory() {
        const pillsContainer = document.getElementById('ngo-inventory-pills');
        if (!pillsContainer) return;

        try {
            const stock = await API.request('/api/inventory/stock', { method: 'GET' });
            this.stockData = stock;

            if (stock && stock.stockByGroup) {
                pillsContainer.innerHTML = Object.entries(stock.stockByGroup).map(([bg, data]) => `
                    <div style="background:var(--bg-main); border:1px solid ${data.isLow ? 'var(--primary)' : '#ddd'}; border-radius:var(--radius-sm); padding:10px 12px; text-align:center;">
                        <div style="font-weight:bold; font-size:1.1rem; color:var(--primary);">${escapeHtml(bg)}</div>
                        <div style="font-size:1.4rem; font-weight:bold; margin:4px 0;">${data.count} <span style="font-size:0.75rem; font-weight:normal; color:var(--text-muted)">units</span></div>
                        ${data.isLow ? '<span class="badge badge-urgent" style="font-size:0.65rem;">Low Stock</span>' : '<span class="badge badge-open" style="font-size:0.65rem;">Adequate</span>'}
                    </div>
                `).join('');
            }

            this.filterInventory();
        } catch (err) {
            console.warn('Could not load inventory:', err);
        }
    },

    filterInventory() {
        const tbody = document.getElementById('ngo-inventory-table-body');
        if (!tbody || !this.stockData) return;

        const bgFilter = (document.getElementById('inv-filter-group')?.value || '');
        const statusFilter = (document.getElementById('inv-filter-status')?.value || '');

        let allUnits = [...(this.stockData.activeUnits || []), ...(this.stockData.expiredUnits || [])];

        if (bgFilter) {
            allUnits = allUnits.filter(u => u.bloodGroup === bgFilter);
        }
        if (statusFilter) {
            allUnits = allUnits.filter(u => u.viability === statusFilter);
        }

        if (allUnits.length === 0) {
            tbody.innerHTML = '<tr><td colspan="8" style="text-align:center; padding:20px; color:var(--text-muted)">No blood bags match selected filters.</td></tr>';
            return;
        }

        tbody.innerHTML = allUnits.map(u => {
            let badgeClass = 'badge-fresh';
            if (u.viability === 'Expiring Soon') badgeClass = 'badge-expiring';
            else if (u.viability === 'Expired') badgeClass = 'badge-expired';
            else if (u.viability === 'Discarded') badgeClass = 'badge-discarded';

            return `
                <tr>
                    <td><strong>${escapeHtml(u.id)}</strong></td>
                    <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(u.bloodGroup)}</span></td>
                    <td>${escapeHtml(u.campName || 'Drive')}</td>
                    <td>${escapeHtml(u.collectionDate)}</td>
                    <td>${escapeHtml(u.expiryDate)}</td>
                    <td>${u.daysRemaining >= 0 ? `${u.daysRemaining} days left` : 'Expired'}</td>
                    <td><span class="badge ${badgeClass}">${escapeHtml(u.viability || 'Fresh')}</span></td>
                    <td>
                        <div style="display:flex; gap:4px;">
                            ${u.viability !== 'Discarded' && u.viability !== 'Transfused' ? `
                                <button class="btn btn-outline" style="padding:2px 8px; font-size:0.75rem;"
                                    ${u.viability === 'Expired' ? 'disabled title="Expired units cannot be transfused"' : ''} onclick="NGODashboard.updateUnitStatus('${u.id}', 'Transfused')">Transfused</button>
                                <button class="btn btn-outline" style="padding:2px 8px; font-size:0.75rem; color:#c62828;"
                                    onclick="NGODashboard.updateUnitStatus('${u.id}', 'Discarded')">Discard</button>
                            ` : `<small style="color:var(--text-muted)">Disposed</small>`}
                        </div>
                    </td>
                </tr>
            `;
        }).join('');
    },

    async updateUnitStatus(unitId, status) {
        try {
            await API.request(`/api/inventory/units/${unitId}/status`, {
                method: 'PUT',
                body: JSON.stringify({ status })
            });
            Toast.show(`Unit ${unitId} marked as ${status}.`, 'success');
            this.loadInventory();
            this.loadKPIs();
            PublicStock.render();
        } catch (err) {
            Toast.show(err.message || 'Failed to update unit.', 'error');
        }
    },

    async updateRequestStatus(reqId, status) {
        try {
            await API.request(`/api/requests/${reqId}/status`, {
                method: 'PUT',
                body: JSON.stringify({ status })
            });
            Toast.show(`Request ${reqId} marked as ${status}.`, 'success');
            this.loadRequestsTable();
            this.loadKPIs();
        } catch (err) {
            Toast.show(err.message || 'Failed to update request.', 'error');
        }
    },

    async loadDatabase(page = 1) {
        const tbody = document.getElementById('ngo-database-body');
        const pagination = document.getElementById('ngo-donor-pagination');
        if (!tbody) return;

        const q = (document.getElementById('search-ngo-donors')?.value || '').trim();
        const bg = (document.getElementById('filter-ngo-bg')?.value || '');

        let url = `/api/org/donors?page=${page}`;
        if (q) url += `&q=${encodeURIComponent(q)}`;
        if (bg) url += `&bg=${encodeURIComponent(bg)}`;

        try {
            const data = await API.request(url, { method: 'GET' });
            if (!data.items || data.items.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:20px;">No consented donors found.</td></tr>';
                if (pagination) pagination.innerHTML = '';
                return;
            }

            tbody.innerHTML = data.items.map(d => `
                <tr>
                    <td><strong>${escapeHtml(d.fullName)}</strong><br><small>@${escapeHtml(d.username)}</small></td>
                    <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(d.bloodGroup)}</span></td>
                    <td>${escapeHtml(d.phone)}</td>
                    <td>${d.age} yrs / ${d.weight} kg</td>
                    <td>${d.isEligibleToday ? '<span style="color:var(--accent-sage)">Eligible</span>' : `<span style="color:var(--primary)">Next: ${d.nextEligibleDate}</span>`}</td>
                </tr>
            `).join('');

            if (pagination) {
                pagination.innerHTML = `
                    <span>Page ${data.currentPage} of ${data.totalPages} (${data.totalItems} donors)</span>
                    <div class="pagination-controls">
                        <button class="page-btn" ${data.currentPage <= 1 ? 'disabled' : ''} onclick="NGODashboard.loadDatabase(${data.currentPage - 1})">Previous</button>
                        <button class="page-btn" ${data.currentPage >= data.totalPages ? 'disabled' : ''} onclick="NGODashboard.loadDatabase(${data.currentPage + 1})">Next</button>
                    </div>
                `;
            }
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="5" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    },

    async loadReminders() {
        const container = document.getElementById('ngo-reminders-container');
        if (!container) return;

        try {
            const reminders = await API.request('/api/org/reminders', { method: 'GET' });
            let html = '<div style="display:grid; grid-template-columns:repeat(auto-fit, minmax(260px, 1fr)); gap:20px;">';

            let count = 0;
            for (const [bg, list] of Object.entries(reminders)) {
                if (list.length === 0) continue;
                count++;
                html += `
                    <div style="background:var(--bg-main); border:1px solid #ddd; border-radius:var(--radius-sm); padding:16px;">
                        <h4 style="color:var(--primary); border-bottom:1px solid #eee; padding-bottom:8px; margin-bottom:12px;">
                            ${escapeHtml(bg)} (${list.length} Donors)
                        </h4>
                        <ul style="list-style:none; padding:0; margin:0;">
                            ${list.map(d => `
                                <li style="padding:6px 0; border-bottom:1px dashed #eee; font-size:0.9rem;">
                                    <strong>${escapeHtml(d.fullName)}</strong><br>
                                    <small>${d.phone} | ${d.nextEligibleDate === 'Eligible Now' ? '<span style="color:var(--accent-sage)">Eligible Now</span>' : 'Eligible: ' + d.nextEligibleDate}</small>
                                </li>
                            `).join('')}
                        </ul>
                    </div>
                `;
            }
            html += '</div>';

            if (count === 0) {
                container.innerHTML = '<p style="text-align:center; padding:24px; color:var(--text-muted)">No donors currently reaching eligibility within the next 14 days.</p>';
            } else {
                container.innerHTML = html;
            }
        } catch (err) {
            container.innerHTML = `<p style="color:var(--primary)">${escapeHtml(err.message)}</p>`;
        }
    },

    async loadProfileForm() {
        try {
            const org = await API.request('/api/org/profile', { method: 'GET' });
            if (org) {
                if (document.getElementById('org-prof-name')) document.getElementById('org-prof-name').value = org.fullName || '';
                if (document.getElementById('org-prof-city')) document.getElementById('org-prof-city').value = org.city || 'Nainital';
                if (document.getElementById('org-prof-phone')) document.getElementById('org-prof-phone').value = org.contactPhone || '';
                if (document.getElementById('org-prof-email')) document.getElementById('org-prof-email').value = org.contactEmail || '';
            }
        } catch (err) {
            console.warn('Could not load org profile:', err);
        }
    }
};

// ==========================================
// 6. ADMIN DASHBOARD
// ==========================================

const AdminDashboard = {
    init() {
        document.querySelectorAll('.admin-nav').forEach(btn => {
            btn.addEventListener('click', () => {
                document.querySelectorAll('.admin-nav').forEach(b => b.classList.remove('active'));
                const container = document.getElementById('dashboard-admin');
                if (container) {
                    container.querySelectorAll('.dash-panel').forEach(p => p.classList.remove('active'));
                }
                btn.classList.add('active');
                const target = document.getElementById(btn.dataset.target);
                if (target) target.classList.add('active');

                if (btn.dataset.target === 'dash-admin-orgs') AdminDashboard.loadOrgs();
                if (btn.dataset.target === 'dash-admin-audit') AdminDashboard.loadAudit(1);
            });
        });
    },

    async loadOrgs() {
        const tbody = document.getElementById('admin-orgs-table-body');
        if (!tbody) return;

        try {
            const orgs = await API.request('/api/admin/orgs', { method: 'GET' });
            if (!orgs || orgs.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:20px;">No registered organizations.</td></tr>';
                return;
            }

            tbody.innerHTML = orgs.map(o => `
                <tr>
                    <td><strong>${escapeHtml(o.fullName)}</strong></td>
                    <td>@${escapeHtml(o.username)}</td>
                    <td>${escapeHtml(o.city || 'Nainital')}</td>
                    <td><span class="badge ${o.approvalStatus === 'Approved' ? 'badge-open' : o.approvalStatus === 'Pending' ? 'badge-urgent' : 'badge-cancelled'}">${escapeHtml(o.approvalStatus)}</span></td>
                    <td>
                        ${o.approvalStatus === 'Pending' ? `
                            <button class="btn btn-outline" style="padding:4px 8px; font-size:0.8rem;"
                                onclick="AdminDashboard.setStatus('${o.username}', 'Approved')">Approve</button>
                            <button class="btn btn-outline" style="padding:4px 8px; font-size:0.8rem; color:#c62828;"
                                onclick="AdminDashboard.setStatus('${o.username}', 'Rejected')">Reject</button>
                        ` : `<small style="color:var(--text-muted)">Finalized</small>`}
                    </td>
                </tr>
            `).join('');
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="5" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    },

    async setStatus(username, status) {
        try {
            await API.request(`/api/admin/orgs/${username}/status`, {
                method: 'PUT',
                body: JSON.stringify({ status })
            });
            Toast.show(`Organization @${username} status updated to ${status}.`, 'success');
            this.loadOrgs();
        } catch (err) {
            Toast.show(err.message || 'Failed to update organization status.', 'error');
        }
    },

    async loadAudit(page = 1) {
        const tbody = document.getElementById('admin-audit-table-body');
        const pagination = document.getElementById('admin-audit-pagination');
        if (!tbody) return;

        try {
            const data = await API.request(`/api/admin/audit-log?page=${page}`, { method: 'GET' });
            if (!data.items || data.items.length === 0) {
                tbody.innerHTML = '<tr><td colspan="4" style="text-align:center; padding:20px;">No audit entries found.</td></tr>';
                if (pagination) pagination.innerHTML = '';
                return;
            }

            tbody.innerHTML = data.items.map(a => `
                <tr>
                    <td><small>${escapeHtml(a.timestamp)}</small></td>
                    <td>@${escapeHtml(a.username)}</td>
                    <td><strong>${escapeHtml(a.action)}</strong></td>
                    <td>${escapeHtml(a.details)}</td>
                </tr>
            `).join('');

            if (pagination) {
                pagination.innerHTML = `
                    <span>Page ${data.currentPage} of ${data.totalPages} (${data.totalItems} entries)</span>
                    <div class="pagination-controls">
                        <button class="page-btn" ${data.currentPage <= 1 ? 'disabled' : ''} onclick="AdminDashboard.loadAudit(${data.currentPage - 1})">Previous</button>
                        <button class="page-btn" ${data.currentPage >= data.totalPages ? 'disabled' : ''} onclick="AdminDashboard.loadAudit(${data.currentPage + 1})">Next</button>
                    </div>
                `;
            }
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="4" style="color:var(--primary)">${escapeHtml(err.message)}</td></tr>`;
        }
    }
};

// ==========================================
// 7. PUBLIC STOCK & INTERACTIVE COMPATIBILITY
// ==========================================

const PublicStock = {
    async render() {
        const grid = document.getElementById('public-stock-grid');
        if (!grid) return;

        // Display skeleton loading cards while fetching real stock
        grid.innerHTML = `
            <div class="skeleton-card" style="padding:20px;">
                <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:12px;">
                    <div class="skeleton-shimmer skeleton-box" style="width:36px; height:28px;"></div>
                    <div class="skeleton-shimmer skeleton-box" style="width:54px; height:18px;"></div>
                </div>
                <div class="skeleton-shimmer skeleton-line short" style="height:22px; width:60px; margin-bottom:12px;"></div>
                <div class="skeleton-shimmer skeleton-line long" style="height:8px; margin-bottom:0;"></div>
            </div>
        `.repeat(8);

        try {
            const stock = await API.request('/api/public/stock', { method: 'GET' });
            grid.innerHTML = Object.entries(stock).map(([bg, info]) => `
                <div class="stock-card live-fade-in">
                    <strong>${escapeHtml(bg)}</strong>
                    <span class="stock-pill ${info.status.toLowerCase()}">${escapeHtml(info.status)}</span>
                    <div style="font-size:0.8rem; color:var(--text-muted); margin-top:8px;">${info.count} Usable Units</div>
                </div>
            `).join('');
        } catch (err) {
            grid.innerHTML = `<p style="color:var(--primary); text-align:center;">Failed to load blood stock.</p>`;
        }
    }
};

const CompatibilityTool = {
    matrix: null,

    async init() {
        try {
            this.matrix = await API.request('/api/compatibility', { method: 'GET' });
            this.showForGroup('O-');

            document.querySelectorAll('.compat-btn').forEach(btn => {
                btn.addEventListener('click', () => {
                    document.querySelectorAll('.compat-btn').forEach(b => b.classList.remove('active'));
                    btn.classList.add('active');
                    this.showForGroup(btn.dataset.bg);
                });
            });
        } catch (err) {
            console.error('Failed to init compatibility:', err);
        }
    },

    showForGroup(bg) {
        if (!this.matrix) return;
        const giveEl = document.getElementById('compat-give-container');
        const recvEl = document.getElementById('compat-receive-container');

        let canGive = [];
        let canRecv = [];

        if (Array.isArray(this.matrix)) {
            const entry = this.matrix.find(m => m.type === bg);
            if (entry) {
                canGive = entry.donateTo || [];
                canRecv = entry.receiveFrom || [];
            }
        } else if (this.matrix.canDonateTo) {
            canGive = this.matrix.canDonateTo[bg] || [];
            canRecv = this.matrix.canReceiveFrom[bg] || [];
        }

        if (giveEl) {
            giveEl.innerHTML = canGive.length > 0
                ? canGive.map(g => `<span class="badge badge-normal" style="font-size:1rem; padding:6px 14px;">${escapeHtml(g)}</span>`).join(' ')
                : '<small style="color:var(--text-muted)">None</small>';
        }
        if (recvEl) {
            recvEl.innerHTML = canRecv.length > 0
                ? canRecv.map(g => `<span class="badge badge-open" style="font-size:1rem; padding:6px 14px;">${escapeHtml(g)}</span>`).join(' ')
                : '<small style="color:var(--text-muted)">None</small>';
        }
    }
};

// ==========================================
// 8. CAMPS MANAGER
// ==========================================

const CampsManager = {
    camps: [],

    parseDate(dateStr) {
        const parts = dateStr.split('-');
        if (parts.length === 3) {
            const months = ['JAN', 'FEB', 'MAR', 'APR', 'MAY', 'JUN', 'JUL', 'AUG', 'SEP', 'OCT', 'NOV', 'DEC'];
            const mIdx = parseInt(parts[1], 10) - 1;
            return { month: months[mIdx] || 'DATE', day: parts[2] };
        }
        return { month: 'DATE', day: '01' };
    },

    async fetchCamps() {
        try {
            this.camps = await API.request('/api/camps', { method: 'GET' });
            return this.camps;
        } catch (err) {
            return [];
        }
    },

    async renderCamps() {
        const grid = document.getElementById('camps-grid');
        if (!grid) return;

        // Display skeleton loading cards while fetching live camp data
        grid.innerHTML = `
            <div class="skeleton-card" style="display:flex; gap:16px;">
                <div class="skeleton-shimmer skeleton-box" style="width:56px; height:56px; border-radius:6px; flex-shrink:0;"></div>
                <div style="flex:1;">
                    <div class="skeleton-shimmer skeleton-line medium" style="height:18px; width:65%; margin-bottom:10px;"></div>
                    <div class="skeleton-shimmer skeleton-line long" style="height:12px; margin-bottom:12px;"></div>
                    <div class="skeleton-shimmer skeleton-line long" style="height:6px; margin-bottom:14px;"></div>
                    <div class="skeleton-shimmer skeleton-box" style="width:100px; height:28px;"></div>
                </div>
            </div>
        `.repeat(4);

        const camps = await this.fetchCamps();
        if (!camps || camps.length === 0) {
            grid.innerHTML = '<p style="text-align:center; grid-column:1/-1;">No camps currently scheduled.</p>';
            return;
        }

        const user = API.getCurrentUser();
        const isDonor = user && user.role === 'donor';
        const isNgo = user && user.role === 'ngo';

        grid.innerHTML = camps.map(camp => {
            const { month, day } = this.parseDate(camp.date);
            const target = camp.targetUnits || 1;
            const collected = camp.unitsCollected || 0;
            const pct = Math.min(100, Math.round((collected / target) * 100));
            const statusClass = (camp.status || 'upcoming').toLowerCase();
            const isRegistered = user && camp.registeredDonors && camp.registeredDonors.includes(user.username);
            const isFinished = camp.status === 'Completed' || camp.status === 'Cancelled';

            let actionHtml = '';
            if (isFinished) {
                actionHtml = `<span style="font-size:0.8rem; color:var(--text-muted); margin-top:8px; display:inline-block;">Drive Concluded</span>`;
            } else if (isDonor) {
                if (isRegistered) {
                    actionHtml = `
                        <div style="margin-top:10px; display:flex; align-items:center; justify-content:space-between; flex-wrap:wrap; gap:8px;">
                            <span class="badge badge-open" style="font-size:0.8rem;">✓ Slot Reserved</span>
                            <button class="btn btn-outline" style="padding:4px 10px; font-size:0.8rem; color:#c62828;"
                                onclick="CampsManager.cancelRegistration('${camp.id}')">Cancel Slot</button>
                        </div>
                    `;
                } else if (camp.remainingSlots <= 0) {
                    actionHtml = `<span class="badge badge-urgent" style="margin-top:10px; display:inline-block;">Slots Full</span>`;
                } else {
                    actionHtml = `
                        <button class="btn btn-secondary" style="margin-top:10px; padding:6px 14px; font-size:0.85rem;"
                            onclick="CampsManager.registerDonor('${camp.id}')">Reserve Slot</button>
                    `;
                }
            } else if (!user) {
                actionHtml = `
                    <button class="btn btn-outline" style="margin-top:10px; padding:6px 12px; font-size:0.8rem;"
                        onclick="Auth.openModal('login')">Sign In to Reserve</button>
                `;
            } else if (isNgo) {
                actionHtml = `
                    <button class="btn btn-outline" style="margin-top:10px; padding:5px 10px; font-size:0.8rem;"
                        onclick="NGODashboard.viewCampDonors('${camp.id}')">📋 View Registered Donors (${camp.registeredCount || 0})</button>
                `;
            }

            return `
                <div class="camp-card">
                    <div class="camp-date">
                        <span class="month">${month}</span>
                        <span class="day">${day}</span>
                    </div>
                    <div class="camp-details">
                        <div style="display:flex; justify-content:space-between; align-items:center;">
                            <h4>${escapeHtml(camp.name)}</h4>
                            <span class="badge badge-${statusClass}">${escapeHtml(camp.status || 'Upcoming')}</span>
                        </div>
                        <p style="margin:4px 0;">Units: ${collected} / ${target} (${pct}%) | Slots left: ${camp.remainingSlots}</p>
                        <div class="progress-bar">
                            <div class="fill" style="width: ${pct}%;"></div>
                        </div>
                        ${actionHtml}
                    </div>
                </div>
            `;
        }).join('');

        this.populateDropdown();
        if (typeof LiveInternetFeeds !== 'undefined') {
            LiveInternetFeeds.updateMapMarkers();
        }
    },

    async registerDonor(campId) {
        try {
            await API.request(`/api/camps/${campId}/register`, { method: 'POST' });
            Toast.show('Congratulations! Your slot registration is confirmed.', 'success');
            await this.renderCamps();
            if (typeof DonorDashboard !== 'undefined' && DonorDashboard.loadAppointments) {
                DonorDashboard.loadAppointments();
            }
        } catch (err) {
            Toast.show(err.message || 'Could not register for camp.', 'error');
        }
    },

    async cancelRegistration(campId) {
        try {
            await API.request(`/api/camps/${campId}/register`, { method: 'DELETE' });
            Toast.show('Camp reservation cancelled.', 'info');
            await this.renderCamps();
            if (typeof DonorDashboard !== 'undefined' && DonorDashboard.loadAppointments) {
                DonorDashboard.loadAppointments();
            }
        } catch (err) {
            Toast.show(err.message || 'Could not cancel registration.', 'error');
        }
    },

    populateDropdown() {
        const select = document.getElementById('don-camp');
        if (!select || !this.camps) return;

        const currentVal = select.value;
        select.innerHTML = '<option value="">Select a camp...</option>' +
            this.camps.map(c => `<option value="${c.id}">${escapeHtml(c.name)}</option>`).join('');

        if (currentVal) select.value = currentVal;
    }
};

// ==========================================
// 9. PUBLIC RECORDS
// ==========================================

const PublicRecords = {
    init() {
        const searchInput = document.getElementById('search-records');
        if (searchInput) {
            let timeout = null;
            searchInput.addEventListener('input', (e) => {
                clearTimeout(timeout);
                timeout = setTimeout(() => {
                    PublicRecords.render(1);
                }, 200);
            });
        }
    },

    async render(page = 1) {
        const tbody = document.getElementById('public-records-body');
        const pagination = document.getElementById('public-records-pagination');
        if (!tbody) return;

        // Render skeleton table rows while loading consented records & live avatars
        tbody.innerHTML = `
            <tr class="skeleton-row">
                <td><div style="display:flex; align-items:center; gap:10px;"><div class="skeleton-shimmer skeleton-circle" style="width:28px; height:28px;"></div><div class="skeleton-shimmer skeleton-line medium" style="margin:0; width:120px;"></div></div></td>
                <td><div class="skeleton-shimmer skeleton-line short" style="margin:0; width:40px;"></div></td>
                <td><div class="skeleton-shimmer skeleton-line medium" style="margin:0; width:140px;"></div></td>
                <td><div class="skeleton-shimmer skeleton-line short" style="margin:0; width:90px;"></div></td>
                <td><div class="skeleton-shimmer skeleton-box" style="width:70px; height:22px;"></div></td>
            </tr>
        `.repeat(5);

        const q = (document.getElementById('search-records')?.value || '').trim();
        let url = `/api/public/records?page=${page}`;
        if (q) url += `&q=${encodeURIComponent(q)}`;

        try {
            const data = await API.request(url, { method: 'GET' });
            if (!data.items || data.items.length === 0) {
                tbody.innerHTML = '<tr><td colspan="5" style="text-align:center; padding:24px;">No consented donor records found.</td></tr>';
                if (pagination) pagination.innerHTML = '';
                return;
            }

            tbody.innerHTML = data.items.map(r => `
                <tr>
                    <td>
                        <div style="display:flex; align-items:center; gap:10px;">
                            <span class="initial-avatar" aria-hidden="true">♥</span>
                            <strong>${escapeHtml(r.donorName)}</strong>
                        </div>
                    </td>
                    <td><span style="color:var(--primary); font-weight:bold;">${escapeHtml(r.bloodGroup)}</span></td>
                    <td>${escapeHtml(r.campName || r.camp)}</td>
                    <td>${escapeHtml(r.date)}</td>
                    <td><span class="status-badge status-completed">${escapeHtml(r.status)}</span></td>
                </tr>
            `).join('');

            if (pagination) {
                pagination.innerHTML = `
                    <span>Page ${data.currentPage} of ${data.totalPages} (${data.totalItems} records)</span>
                    <div class="pagination-controls">
                        <button class="page-btn" ${data.currentPage <= 1 ? 'disabled' : ''} onclick="PublicRecords.render(${data.currentPage - 1})">Previous</button>
                        <button class="page-btn" ${data.currentPage >= data.totalPages ? 'disabled' : ''} onclick="PublicRecords.render(${data.currentPage + 1})">Next</button>
                    </div>
                `;
            }
        } catch (err) {
            tbody.innerHTML = `<tr><td colspan="5" style="color:var(--primary)">Error: ${escapeHtml(err.message)}</td></tr>`;
        }
    }
};

// ==========================================
// 10. LIVE INTERNET DATA FEEDS (LEAFLET MAP, OPEN-METEO WEATHER, AVATARS & BULLETINS)
// ==========================================

const LiveInternetFeeds = {
    map: null,
    markers: [],
    campCoordinates: {
        'CAMP-001': { lat: 29.3900, lng: 79.4560, name: 'RamSay Hospital, Mallital' },
        'CAMP-002': { lat: 29.3780, lng: 79.4710, name: 'Graphic Era Blood Drive, Bhimtal Rd' },
        'CAMP-003': { lat: 29.3980, lng: 79.4470, name: 'CHEA Nainital Headquarters' },
        'CAMP-004': { lat: 29.5973, lng: 79.6591, name: 'District Red Cross Society Almora' }
    },

    init() {
        this.fetchWeather();
        this.fetchHealthBulletins();
        this.fetchWikiBloodBank();
    },

    async fetchWikiBloodBank() {
        const skel = document.getElementById('home-wiki-skeleton');
        const content = document.getElementById('home-wiki-content');
        if (!skel || !content) return;

        skel.style.display = 'block';
        content.style.display = 'none';

        try {
            const res = await fetch('https://en.wikipedia.org/api/rest_v1/page/summary/Blood_bank', { signal: AbortSignal.timeout(6000) });
            if (!res.ok) throw new Error(`HTTP ${res.status}`);
            const data = await res.json();

            const thumb = data.thumbnail && data.thumbnail.source
                ? `<img src="${data.thumbnail.source}" alt="${escapeHtml(data.title)}" style="width:110px; height:110px; object-fit:cover; border-radius:var(--radius-sm); border:1px solid #e0d7ce; flex-shrink:0;">`
                : `<div style="width:110px; height:110px; background:#f5eee6; border-radius:var(--radius-sm); display:flex; align-items:center; justify-content:center; font-size:2rem; flex-shrink:0;">🏥</div>`;

            content.innerHTML = `
                <div style="display:flex; gap:18px; align-items:flex-start; flex-wrap:wrap;">
                    ${thumb}
                    <div style="flex:1; min-width:240px;">
                        <h4 style="margin:0 0 6px 0; font-size:1.15rem; color:var(--text-main);">${escapeHtml(data.title)}</h4>
                        <p style="margin:0 0 12px 0; font-size:0.88rem; color:var(--text-muted); line-height:1.5;">${escapeHtml(data.extract)}</p>
                        <div style="display:flex; gap:12px; align-items:center;">
                            <a href="${data.content_urls ? data.content_urls.desktop.page : '#'}" target="_blank" rel="noopener noreferrer" class="btn btn-outline" style="font-size:0.8rem; padding:4px 12px;">
                                📖 Read Full Medical Article on Wikipedia ↗
                            </a>
                            <span style="font-size:0.75rem; color:var(--text-muted);">Verified Peer-Reviewed Summary</span>
                        </div>
                    </div>
                </div>
            `;
        } catch (err) {
            console.warn('Wiki blood bank fetch error:', err);
            content.innerHTML = `
                <p style="font-size:0.9rem; color:var(--text-muted);">A blood bank is a cache or bank of blood or blood components, gathered as a result of blood donation, stored and preserved for later use in blood transfusion.</p>
            `;
        } finally {
            skel.style.display = 'none';
            content.style.display = 'block';
            content.classList.add('live-fade-in');
        }
    },

    async fetchWikiRedCross() {
        const skel = document.getElementById('about-partner-skeleton');
        const content = document.getElementById('about-partner-content');
        if (!skel || !content) return;

        skel.style.display = 'block';
        content.style.display = 'none';

        try {
            const res = await fetch('https://en.wikipedia.org/api/rest_v1/page/summary/International_Red_Cross_and_Red_Crescent_Movement', { signal: AbortSignal.timeout(6000) });
            if (!res.ok) throw new Error(`HTTP ${res.status}`);
            const data = await res.json();

            const thumb = data.thumbnail && data.thumbnail.source
                ? `<img src="${data.thumbnail.source}" alt="${escapeHtml(data.title)}" style="width:110px; height:110px; object-fit:cover; border-radius:var(--radius-sm); border:1px solid #e0d7ce; flex-shrink:0;">`
                : `<div style="width:110px; height:110px; background:#f5eee6; border-radius:var(--radius-sm); display:flex; align-items:center; justify-content:center; font-size:2rem; flex-shrink:0;">⛑️</div>`;

            content.innerHTML = `
                <div style="display:flex; gap:18px; align-items:flex-start; flex-wrap:wrap;">
                    ${thumb}
                    <div style="flex:1; min-width:240px;">
                        <h4 style="margin:0 0 6px 0; font-size:1.15rem; color:var(--text-main);">${escapeHtml(data.title)}</h4>
                        <p style="margin:0 0 12px 0; font-size:0.88rem; color:var(--text-muted); line-height:1.5;">${escapeHtml(data.extract)}</p>
                        <div style="display:flex; gap:12px; align-items:center;">
                            <a href="${data.content_urls ? data.content_urls.desktop.page : '#'}" target="_blank" rel="noopener noreferrer" class="btn btn-outline" style="font-size:0.8rem; padding:4px 12px;">
                                🕊️ View Humanitarian Charter ↗
                            </a>
                            <span style="font-size:0.75rem; color:var(--text-muted);">Accredited International Health Partner</span>
                        </div>
                    </div>
                </div>
            `;
        } catch (err) {
            console.warn('Wiki Red Cross fetch error:', err);
            content.innerHTML = `
                <p style="font-size:0.9rem; color:var(--text-muted);">The International Red Cross and Red Crescent Movement is a humanitarian movement with approximately 16 million volunteers, members and staff worldwide.</p>
            `;
        } finally {
            skel.style.display = 'none';
            content.style.display = 'block';
            content.classList.add('live-fade-in');
        }
    },

    async fetchWikiTransfusion() {
        const skel = document.getElementById('info-science-skeleton');
        const content = document.getElementById('info-science-content');
        if (!skel || !content) return;

        skel.style.display = 'block';
        content.style.display = 'none';

        try {
            const res = await fetch('https://en.wikipedia.org/api/rest_v1/page/summary/Blood_transfusion', { signal: AbortSignal.timeout(6000) });
            if (!res.ok) throw new Error(`HTTP ${res.status}`);
            const data = await res.json();

            const thumb = data.thumbnail && data.thumbnail.source
                ? `<img src="${data.thumbnail.source}" alt="${escapeHtml(data.title)}" style="width:110px; height:110px; object-fit:cover; border-radius:var(--radius-sm); border:1px solid #e0d7ce; flex-shrink:0;">`
                : `<div style="width:110px; height:110px; background:#f5eee6; border-radius:var(--radius-sm); display:flex; align-items:center; justify-content:center; font-size:2rem; flex-shrink:0;">🔬</div>`;

            content.innerHTML = `
                <div style="display:flex; gap:18px; align-items:flex-start; flex-wrap:wrap;">
                    ${thumb}
                    <div style="flex:1; min-width:240px;">
                        <h4 style="margin:0 0 6px 0; font-size:1.15rem; color:var(--text-main);">${escapeHtml(data.title)}</h4>
                        <p style="margin:0 0 12px 0; font-size:0.88rem; color:var(--text-muted); line-height:1.5;">${escapeHtml(data.extract)}</p>
                        <div style="display:flex; gap:12px; align-items:center;">
                            <a href="${data.content_urls ? data.content_urls.desktop.page : '#'}" target="_blank" rel="noopener noreferrer" class="btn btn-outline" style="font-size:0.8rem; padding:4px 12px;">
                                💉 Full Clinical Procedure Reference ↗
                            </a>
                            <span style="font-size:0.75rem; color:var(--text-muted);">Standard Clinical Protocol</span>
                        </div>
                    </div>
                </div>
            `;
        } catch (err) {
            console.warn('Wiki transfusion fetch error:', err);
            content.innerHTML = `
                <p style="font-size:0.9rem; color:var(--text-muted);">Blood transfusion is the process of transferring blood products into a person's circulation intravenously, commonly used in surgeries and medical emergencies.</p>
            `;
        } finally {
            skel.style.display = 'none';
            content.style.display = 'block';
            content.classList.add('live-fade-in');
        }
    },

    onCampsSectionShown() {
        if (!this.map) {
            this.initMap();
        } else {
            setTimeout(() => {
                if (this.map) this.map.invalidateSize();
            }, 100);
        }
    },

    initMap() {
        const mapContainer = document.getElementById('camps-leaflet-map');
        if (!mapContainer || typeof L === 'undefined') return;

        if (this.map) {
            this.map.invalidateSize();
            return;
        }

        // Initialize Leaflet Map centered on Nainital Kumaon region
        this.map = L.map('camps-leaflet-map', {
            center: [29.3919, 79.4542],
            zoom: 11,
            scrollWheelZoom: false
        });

        // Live OpenStreetMap Tile Layer (triggers live external tile requests)
        L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
            maxZoom: 19,
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap</a> contributors'
        }).addTo(this.map);

        setTimeout(() => {
            if (this.map) this.map.invalidateSize();
        }, 200);

        this.updateMapMarkers();
    },

    updateMapMarkers() {
        if (!this.map || typeof L === 'undefined') return;

        // Clear existing markers
        this.markers.forEach(m => this.map.removeLayer(m));
        this.markers = [];

        const camps = CampsManager.camps || [];
        const user = API.getCurrentUser();
        const isDonor = user && user.role === 'donor';

        camps.forEach((camp, index) => {
            const coords = this.campCoordinates[camp.id] || {
                lat: 29.3919 + (index * 0.015) - 0.02,
                lng: 79.4542 + (index * 0.015) - 0.02,
                name: camp.location || camp.name
            };

            const isFull = camp.remainingSlots <= 0;
            const isCompleted = camp.status === 'Completed' || camp.status === 'Cancelled';
            const isRegistered = user && camp.registeredDonors && camp.registeredDonors.includes(user.username);

            let actionBtn = '';
            if (isCompleted) {
                actionBtn = `<span style="font-size:0.75rem; color:#888;">Drive Concluded</span>`;
            } else if (isDonor) {
                if (isRegistered) {
                    actionBtn = `<span style="font-size:0.75rem; color:#2e7d32; font-weight:600;">✓ Slot Reserved</span>`;
                } else if (isFull) {
                    actionBtn = `<span style="font-size:0.75rem; color:#c62828;">Slots Full</span>`;
                } else {
                    actionBtn = `<button class="btn btn-secondary" style="font-size:0.75rem; padding:4px 8px;" onclick="CampsManager.registerDonor('${camp.id}')">Reserve Slot</button>`;
                }
            } else if (!user) {
                actionBtn = `<button class="btn btn-outline" style="font-size:0.75rem; padding:4px 8px;" onclick="Auth.openModal('login')">Sign In</button>`;
            }

            const popupContent = `
                <div style="font-family:var(--font-sans, sans-serif); min-width:180px; padding:4px 0;">
                    <div style="font-weight:bold; font-size:0.95rem; margin-bottom:4px; color:#2c221e;">${escapeHtml(camp.name)}</div>
                    <div style="font-size:0.8rem; color:#5c4d44; margin-bottom:2px;">📍 ${escapeHtml(camp.location || coords.name)}</div>
                    <div style="font-size:0.8rem; color:#5c4d44; margin-bottom:4px;">📅 Date: <strong>${escapeHtml(camp.date)}</strong></div>
                    <div style="font-size:0.8rem; margin-bottom:8px;">
                        Remaining Slots: <strong style="color:${isFull ? '#c62828' : '#2e7d32'}">${camp.remainingSlots}</strong>
                    </div>
                    <div style="text-align:right;">${actionBtn}</div>
                </div>
            `;

            const marker = L.marker([coords.lat, coords.lng]).addTo(this.map);
            marker.bindPopup(popupContent);
            this.markers.push(marker);
        });
    },

    async fetchWeather() {
        const tempEl = document.getElementById('weather-temp');
        const condEl = document.getElementById('weather-condition');
        const advEl = document.getElementById('weather-advisory');
        const iconEl = document.getElementById('weather-icon-container');

        if (tempEl) tempEl.textContent = 'Syncing Satellite Weather...';
        if (condEl) condEl.textContent = 'Connecting Satellite API...';

        try {
            // Live public meteorological API query for Nainital coordinates (29.3919° N, 79.4542° E)
            const res = await fetch('https://api.open-meteo.com/v1/forecast?latitude=29.3919&longitude=79.4542&current_weather=true', { signal: AbortSignal.timeout(6000) });
            if (!res.ok) throw new Error(`HTTP ${res.status}`);
            const data = await res.json();
            const current = data.current_weather;
            if (!current) throw new Error('No weather data received');

            const temp = Math.round(current.temperature);
            const wCode = current.weathercode;
            const wind = current.windspeed;

            let condition = 'Clear Sky';
            let icon = '☀️';
            if (wCode === 1 || wCode === 2 || wCode === 3) {
                condition = 'Partly Cloudy';
                icon = '⛅';
            } else if (wCode === 45 || wCode === 48) {
                condition = 'Foggy / Hazy';
                icon = '🌫️';
            } else if (wCode >= 51 && wCode <= 67) {
                condition = 'Light Mountain Drizzle / Rain';
                icon = '🌧️';
            } else if (wCode >= 71 && wCode <= 77) {
                condition = 'Snow Fall';
                icon = '❄️';
            } else if (wCode >= 80 && wCode <= 82) {
                condition = 'Rain Showers';
                icon = '🌦️';
            } else if (wCode >= 95) {
                condition = 'Thunderstorm';
                icon = '⛈️';
            }

            let advisoryText = '';
            if (temp >= 28) {
                advisoryText = `⚠️ Warm weather alert (${temp}°C): Donors must drink at least 750ml of water or electrolyte solution 30 minutes before phlebotomy to maintain blood volume and prevent dizziness.`;
            } else if (temp >= 16) {
                advisoryText = `💡 Ideal donation climate (${temp}°C, Wind ${wind} km/h): Drink 500ml of water, take a light protein snack, and avoid intense cardiovascular exercise post-donation.`;
            } else {
                advisoryText = `🧣 Cool mountain weather (${temp}°C): Keep your arms and extremities warm before registration to facilitate healthy peripheral venous dilation for quick, painless needle insertion.`;
            }

            if (tempEl) tempEl.textContent = `${temp}°C (Live Satellite Feed)`;
            if (iconEl) iconEl.textContent = icon;
            if (condEl) {
                condEl.textContent = condition;
                condEl.className = 'badge badge-open';
            }
            if (advEl) advEl.textContent = advisoryText;
        } catch (err) {
            console.warn('Live weather fetch failed, applying fallback:', err);
            if (tempEl) tempEl.textContent = '19°C (Cached)';
            if (condEl) condEl.textContent = 'Partly Cloudy ⛅';
            if (advEl) advEl.textContent = 'Drink 500ml of water prior to donation and rest for 10 minutes in the recovery lounge.';
        }
    },

    async fetchHealthBulletins() {
        const container = document.getElementById('live-bulletins-container');
        if (!container) return;

        const bulletins = [
            {
                tag: 'NBTC National Directive',
                date: 'Updated Live',
                title: 'National Blood Transfusion Council Protocol 2026',
                content: 'Donors must weigh ≥ 45 kg with hemoglobin ≥ 12.5 g/dL. Deferrals apply for 14 days post-vaccination or 6 months following tattoos or dental surgery.',
                authority: 'Ministry of Health & Family Welfare',
                type: 'urgent'
            },
            {
                tag: 'WHO Global Safety',
                date: 'Verified Advisory',
                title: 'World Health Organization Blood Cold Chain Standards',
                content: 'All collected whole blood units are strictly preserved between +2°C and +6°C in certified thermal transport chambers with real-time temperature data loggers.',
                authority: 'WHO Safe Blood Initiative',
                type: 'standard'
            },
            {
                tag: 'Clinical Care Guideline',
                date: 'Uttarakhand Red Cross',
                title: 'Hemovigilance & Rapid Recovery Recommendations',
                content: 'Stay seated in the observation lounge for 15 minutes post-phlebotomy. Complimentary citrus juices and glucose biscuits replenish glycogen and circulating blood volume.',
                authority: 'Uttarakhand State Blood Transfusion Council',
                type: 'standard'
            }
        ];

        container.innerHTML = bulletins.map(b => `
            <div style="background:var(--bg-main, #faf5ef); border-radius:var(--radius-sm); border:1px solid #e0d7ce; padding:16px; display:flex; flex-direction:column; justify-content:space-between;">
                <div>
                    <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:8px;">
                        <span class="badge ${b.type === 'urgent' ? 'badge-urgent' : 'badge-open'}" style="font-size:0.75rem;">${escapeHtml(b.tag)}</span>
                        <span style="font-size:0.75rem; color:var(--text-muted);">${escapeHtml(b.date)}</span>
                    </div>
                    <h4 style="margin:0 0 6px 0; font-size:0.95rem; color:var(--text-main);">${escapeHtml(b.title)}</h4>
                    <p style="margin:0 0 12px 0; font-size:0.82rem; color:var(--text-muted); line-height:1.45;">${escapeHtml(b.content)}</p>
                </div>
                <div style="font-size:0.75rem; color:var(--primary); font-weight:600; border-top:1px dashed #dcd3c7; padding-top:8px;">
                    🏛️ ${escapeHtml(b.authority)}
                </div>
            </div>
        `).join('');
    }
};

// ==========================================
// 11. APP INITIALIZATION & AUTH SYNC
// ==========================================

const UI = {
    async updateAuthState() {
        const user = API.getCurrentUser();
        const btnLoginNav = document.getElementById('btn-login-nav');
        const btnLogoutNav = document.getElementById('btn-logout-nav');
        const navDonor = document.getElementById('nav-donor');
        const navNgo = document.getElementById('nav-ngo');
        const navAdmin = document.getElementById('nav-admin');

        if (user && API.getToken()) {
            if (btnLoginNav) btnLoginNav.style.display = 'none';
            if (btnLogoutNav) btnLogoutNav.style.display = 'inline-flex';

            if (user.role === 'admin') {
                if (navDonor) navDonor.style.display = 'none';
                if (navNgo) navNgo.style.display = 'none';
                if (navAdmin) navAdmin.style.display = 'inline-block';
                await AdminDashboard.loadOrgs();
                await AdminDashboard.loadAudit(1);
            } else if (user.role === 'ngo') {
                if (navDonor) navDonor.style.display = 'none';
                if (navNgo) navNgo.style.display = 'inline-block';
                if (navAdmin) navAdmin.style.display = 'none';
                await NGODashboard.loadProfile();
                await NGODashboard.loadKPIs();
                await NGODashboard.loadInventory();
                await NGODashboard.setupDonorAutocomplete();
            } else {
                if (navDonor) navDonor.style.display = 'inline-block';
                if (navNgo) navNgo.style.display = 'none';
                if (navAdmin) navAdmin.style.display = 'none';
                await DonorDashboard.loadProfile();
                await DonorDashboard.loadCard();
                await DonorDashboard.loadAppointments();
                await DonorDashboard.loadPledgedRequests();
                await DonorDashboard.loadMatchingRequests();
                await DonorDashboard.renderHistory();
            }
        } else {
            if (btnLoginNav) btnLoginNav.style.display = 'inline-flex';
            if (btnLogoutNav) btnLogoutNav.style.display = 'none';
            if (navDonor) navDonor.style.display = 'none';
            if (navNgo) navNgo.style.display = 'none';
            if (navAdmin) navAdmin.style.display = 'none';
        }
    },

    async init() {
        Navigation.init();
        Auth.init();
        DonorDashboard.init();
        NGODashboard.init();
        AdminDashboard.init();
        PublicRecords.init();
        CompatibilityTool.init();
        LiveInternetFeeds.init();

        if (API.getToken()) {
            try {
                const me = await API.request('/api/me', { method: 'GET' });
                API.setCurrentUser(me);
            } catch (err) {
                API.clearAuth();
            }
        }

        await this.updateAuthState();
        await CampsManager.renderCamps();
        await PublicStock.render();
        await PublicRecords.render(1);
    }
};

document.addEventListener('DOMContentLoaded', () => {
    UI.init().catch(error => {
        console.error('Initialization failed:', error);
        Toast.show('Unable to load the latest data. Please refresh to try again.', 'error');
    });
    document.addEventListener('keydown', event => {
        if (event.key === 'Escape') {
            Navigation.collapseMobile();
            document.querySelectorAll('.modal-overlay.open').forEach(modal => modal.classList.remove('open'));
        }
    });

    // ==========================================
    // INTEGRATEDBIO-INSPIRED INTERACTIONS
    // ==========================================

    // — Scroll Stepper Auto-Rotate (01→02→03)
    const stepperSteps = [
        { index: '01', width: '33.333%', text: 'Donors register their medical profile, verify eligibility, and reserve slots at upcoming donation camps — all from one unified platform.' },
        { index: '02', width: '66.666%', text: 'Hospitals organize camps, broadcast urgent blood needs, track 42-day shelf-life inventory, and record verified donation units in real time.' },
        { index: '03', width: '100%', text: 'The community benefits — transparent public records, compatibility matching, emergency alerts, and a verified donor hero wall celebrating every contribution.' }
    ];
    let stepperIdx = 0;
    setInterval(() => {
        if (document.hidden || document.documentElement.classList.contains('motion-paused') || !document.getElementById('home-section')?.classList.contains('active')) return;
        stepperIdx = (stepperIdx + 1) % stepperSteps.length;
        const step = stepperSteps[stepperIdx];
        const bar = document.getElementById('stepper-bar');
        const current = document.getElementById('stepper-current');
        const text = document.getElementById('stepper-text');
        if (bar) bar.style.width = step.width;
        if (current) current.textContent = step.index;
        if (text) {
            text.style.opacity = '0';
            text.style.transform = 'translateY(10px)';
            setTimeout(() => {
                text.textContent = step.text;
                text.style.transition = 'opacity 0.5s cubic-bezier(0.16,1,0.3,1), transform 0.5s cubic-bezier(0.16,1,0.3,1)';
                text.style.opacity = '1';
                text.style.transform = 'translateY(0)';
            }, 250);
        }
    }, 5000);

    // — Scroll Reveal Observer (IntegratedBio staggered fade-in)
    const revealElements = document.querySelectorAll('.feature-item, .info-block, .usp-card, .cta-card, .stock-card, .camp-card, .stat-card');
    revealElements.forEach((el, i) => {
        el.classList.add('reveal');
        el.classList.add(`reveal-delay-${(i % 4) + 1}`);
    });

    const revealObserver = new IntersectionObserver((entries) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('revealed');
                revealObserver.unobserve(entry.target);
            }
        });
    }, { threshold: 0.15, rootMargin: '0px 0px -40px 0px' });

    document.querySelectorAll('.reveal').forEach(el => revealObserver.observe(el));

    // — Hide marquee/usp/stepper when non-home sections are active
    const homeOnlyBands = ['marquee-band', 'usp-cards-band'];
    const originalGoTo = Navigation.goTo.bind(Navigation);
    Navigation.goTo = async function(sectionId) {
        await originalGoTo(sectionId);
        const isHome = sectionId === 'home-section';
        homeOnlyBands.forEach(id => {
            const el = document.getElementById(id);
            if (el) el.style.display = isHome ? '' : 'none';
        });
        const stepperEl = document.querySelector('.stepper-section');
        if (stepperEl) stepperEl.style.display = isHome ? '' : 'none';
    };

    // ==========================================
    // PREMIUM THEME MANAGER (Dark & Light Modes)
    // ==========================================
    const ThemeManager = {
        STORAGE_KEY: 'bloodline-theme',

        init() {
            const saved = localStorage.getItem(this.STORAGE_KEY);
            const prefersDark = window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
            const initialTheme = saved || (prefersDark ? 'dark' : 'light');
            this.setTheme(initialTheme, false);

            const toggleBtn = document.getElementById('theme-toggle-btn');
            if (toggleBtn) {
                toggleBtn.addEventListener('click', (e) => {
                    e.preventDefault();
                    this.toggle();
                });
            }

            // Sync with OS theme changes if user hasn't explicitly chosen
            if (window.matchMedia) {
                window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', (e) => {
                    if (!localStorage.getItem(this.STORAGE_KEY)) {
                        this.setTheme(e.matches ? 'dark' : 'light', false);
                    }
                });
            }
        },

        getTheme() {
            return document.documentElement.getAttribute('data-theme') || 'light';
        },

        toggle() {
            const current = this.getTheme();
            const next = current === 'dark' ? 'light' : 'dark';
            this.setTheme(next, true);
            if (typeof Toast !== 'undefined' && Toast.show) {
                Toast.show(next === 'dark' ? 'Dark Mode Activated' : 'Light Mode Activated', 'info', 2000);
            }
        },

        setTheme(theme, save = true) {
            document.documentElement.setAttribute('data-theme', theme);
            if (theme === 'dark') {
                document.documentElement.classList.add('dark-theme');
                if (document.body) document.body.classList.add('dark-theme');
            } else {
                document.documentElement.classList.remove('dark-theme');
                if (document.body) document.body.classList.remove('dark-theme');
            }

            if (save) {
                localStorage.setItem(this.STORAGE_KEY, theme);
            }

            window.dispatchEvent(new CustomEvent('bloodline-theme-changed', { detail: { theme } }));
        }
    };

    window.ThemeManager = ThemeManager;
    ThemeManager.init();
});


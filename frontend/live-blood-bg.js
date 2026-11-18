/** BloodLine ambient blood-cell field. Cached sprites, bounded resolution, one animation loop. */
(() => {
  'use strict';
  const host = document.getElementById('live-blood-bg-canvas');
  if (!host) return;
  const canvas = document.createElement('canvas');
  host.appendChild(canvas);
  const ctx = canvas.getContext('2d', { alpha: true });
  if (!ctx) return; // CSS ambient lighting remains as a fallback.
  const motion = matchMedia('(prefers-reduced-motion: reduce)');
  const small = matchMedia('(max-width: 760px)');
  const button = document.getElementById('motion-toggle');
  let paused = false;
  try { paused = localStorage.getItem('bloodline-motion') === 'paused'; } catch (_) {}
  let width = 0, height = 0, frame = 0, previous = 0, elapsed = 0;
  let pointerX = 0.72, pointerY = 0.48, smoothX = 0.72, smoothY = 0.48;
  let dark = document.documentElement.dataset.theme === 'dark';
  let slowFrames = 0, limited = false;
  const sprite = document.createElement('canvas');
  sprite.width = sprite.height = 192;
  const paint = sprite.getContext('2d');
  // A translucent, biconcave red-cell disc, painted only once.
  const gradient = paint.createRadialGradient(86, 80, 5, 96, 96, 82);
  gradient.addColorStop(0, '#88152f');
  gradient.addColorStop(.28, '#b62d48');
  gradient.addColorStop(.51, '#f47785');
  gradient.addColorStop(.72, '#de3157');
  gradient.addColorStop(.89, '#a50d38');
  gradient.addColorStop(1, 'rgba(126,8,39,0)');
  paint.fillStyle = gradient;
  paint.beginPath(); paint.arc(96, 96, 83, 0, Math.PI * 2); paint.fill();
  paint.strokeStyle = 'rgba(255,220,224,.42)'; paint.lineWidth = 2;
  paint.beginPath(); paint.ellipse(94, 91, 64, 63, 0, 3.55, 5.7); paint.stroke();
  const cells = Array.from({ length: 26 }, (_, i) => ({
    x: ((i * .61803398875) % 1), y: ((i * .41421356) % 1),
    radius: 18 + ((i * 17) % 53), speed: .008 + (i % 5) * .002,
    angle: i * 2.4, depth: .25 + (i % 6) / 8
  }));
  function draw() {
    ctx.clearRect(0, 0, width, height);
    const count = small.matches || limited ? 14 : cells.length;
    for (let i = 0; i < count; i++) {
      const cell = cells[i];
      const progress = (cell.y - elapsed * cell.speed) % 1;
      const x = (cell.x * .67 + .33) * width + Math.sin(elapsed * .18 + i) * 22 + (smoothX - .5) * cell.depth * 44;
      const y = ((progress + 1) % 1) * (height + 200) - 100 + (smoothY - .5) * cell.depth * 24;
      const radius = cell.radius * (small.matches ? .67 : 1);
      ctx.save();
      ctx.translate(x, y);
      ctx.rotate(cell.angle + elapsed * .06 * (i % 2 ? 1 : -1));
      ctx.scale(1, .52 + Math.sin(elapsed * .25 + i) * .18);
      ctx.globalAlpha = (dark ? .35 : .32) * cell.depth * (x < width * .48 ? .38 : 1);
      ctx.drawImage(sprite, -radius, -radius, radius * 2, radius * 2);
      ctx.restore();
    }
    // Sparse plasma particles: no blur filters or per-frame gradient creation.
    ctx.fillStyle = dark ? 'rgba(255,158,178,.25)' : 'rgba(183,45,76,.18)';
    for (let i = 0; i < 32; i++) {
      const x = ((i * .618 + elapsed * .003) % 1) * width;
      const y = ((i * .327 + elapsed * .004) % 1) * height;
      ctx.beginPath(); ctx.arc(x, y, i % 3 === 0 ? 1.8 : .8, 0, Math.PI * 2); ctx.fill();
    }
  }
  function running() { return !paused && !motion.matches && !document.hidden; }
  function tick(now) {
    frame = 0;
    if (!running()) { sync(); return; }
    const interval = small.matches || limited ? 1000 / 30 : 1000 / 60;
    const delta = now - previous;
    if (delta >= interval - 1) {
      if (previous && delta > 45 && ++slowFrames > 45) limited = true;
      elapsed += previous ? Math.min(delta / 1000, .05) : 0;
      previous = now;
      smoothX += (pointerX - smoothX) * .035;
      smoothY += (pointerY - smoothY) * .035;
      draw();
    }
    frame = requestAnimationFrame(tick);
  }
  function sync() {
    cancelAnimationFrame(frame); frame = 0; previous = 0;
    document.documentElement.classList.toggle('motion-paused', paused || motion.matches);
    if (button) {
      button.textContent = motion.matches ? 'Motion reduced' : paused ? 'Play ambience' : 'Pause ambience';
      button.setAttribute('aria-pressed', String(paused || motion.matches));
      button.disabled = motion.matches;
    }
    draw();
    if (running()) frame = requestAnimationFrame(tick);
  }
  function resize() {
    width = innerWidth; height = innerHeight;
    const ratio = Math.min(devicePixelRatio || 1, small.matches ? 1 : 1.5);
    canvas.width = Math.round(width * ratio); canvas.height = Math.round(height * ratio);
    ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
    draw();
  }
  let resizeTimer;
  addEventListener('resize', () => { clearTimeout(resizeTimer); resizeTimer = setTimeout(resize, 120); }, { passive: true });
  addEventListener('pointermove', e => {
    if (e.pointerType === 'touch') return;
    pointerX = e.clientX / width; pointerY = e.clientY / height;
  }, { passive: true });
  document.addEventListener('visibilitychange', sync);
  addEventListener('focus', sync);
  motion.addEventListener('change', sync);
  small.addEventListener('change', resize);
  addEventListener('bloodline-theme-changed', e => { dark = e.detail.theme === 'dark'; draw(); });
  if (button) button.addEventListener('click', () => {
    paused = !paused;
    try { localStorage.setItem('bloodline-motion', paused ? 'paused' : 'playing'); } catch (_) {}
    sync();
  });
  resize(); sync();
})();

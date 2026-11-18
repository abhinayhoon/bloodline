/**
 * BloodLine — Live 3D Biological Background Engine
 * Exact Clone of IntegratedBio.com in the Theme of Living Blood & Hemodynamics.
 * 
 * Features:
 * - Sweeping 3D parametric horn/funnel biological tissue ribbon (GLSL Shader with Fresnel specular rim & subsurface scattering)
 * - Anatomically accurate Evans-Fung biconcave disc erythrocytes (Red Blood Cells via LatheGeometry)
 * - Dual-peak arterial systolic pulse wave (72 BPM heartbeat rhythm)
 * - Flowing plasma micro-platelets & bioluminescent ambient dust
 * - Interactive mouse parallax & fluid ripple disturbance
 * - Web Audio API synthesized medical heartbeat sound (optional toggle)
 * - IntegratedBio-inspired telemetry control pill (BPM & hemodynamic mode switcher)
 */

(function () {
    'use strict';

    // Audio context for authentic heartbeat synthesis
    let audioCtx = null;
    let isAudioMuted = true;

    function playHeartbeatSound() {
        if (isAudioMuted) return;
        try {
            if (!audioCtx) {
                audioCtx = new (window.AudioContext || window.webkitAudioContext)();
            }
            if (audioCtx.state === 'suspended') {
                audioCtx.resume();
            }
            const now = audioCtx.currentTime;
            // S1 "Lub" sound (~62Hz low resonant thump)
            createCardiacThump(now, 64, 0.13, 0.35);
            // S2 "Dub" sound (~88Hz snappier thump, 0.15s later)
            createCardiacThump(now + 0.15, 86, 0.10, 0.28);
        } catch (e) {
            // Audio context not allowed or supported
        }
    }

    function createCardiacThump(time, freq, duration, maxGain) {
        const osc = audioCtx.createOscillator();
        const gain = audioCtx.createGain();
        const filter = audioCtx.createBiquadFilter();

        osc.type = 'sine';
        osc.frequency.setValueAtTime(freq, time);
        osc.frequency.exponentialRampToValueAtTime(28, time + duration);

        filter.type = 'lowpass';
        filter.frequency.setValueAtTime(130, time);

        gain.gain.setValueAtTime(0.001, time);
        gain.gain.exponentialRampToValueAtTime(maxGain, time + 0.025);
        gain.gain.exponentialRampToValueAtTime(0.0001, time + duration);

        osc.connect(filter);
        filter.connect(gain);
        gain.connect(audioCtx.destination);

        osc.start(time);
        osc.stop(time + duration);
    }

    function initWhenReady() {
        if (typeof THREE === 'undefined') {
            setTimeout(initWhenReady, 40);
            return;
        }
        initBloodBackground();
    }

    function initBloodBackground() {
        const container = document.getElementById('live-blood-bg-canvas');
        if (!container) return;

        // Configuration state
        const config = {
            bpm: 72,
            cellCount: 38,
            particleCount: 180,
            systoleIntensity: 1.0,
            flowSpeed: 0.95
        };

        let width = container.clientWidth || window.innerWidth;
        let height = container.clientHeight || window.innerHeight;

        // Three.js Scene, Camera, Renderer
        const scene = new THREE.Scene();
        const camera = new THREE.PerspectiveCamera(40, width / height, 0.1, 100);
        camera.position.set(0.2, 0.3, 7.5);

        const renderer = new THREE.WebGLRenderer({
            antialias: true,
            alpha: true,
            powerPreference: 'high-performance'
        });
        renderer.setSize(width, height);
        renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
        renderer.toneMapping = THREE.ACESFilmicToneMapping;
        renderer.toneMappingExposure = 1.1;
        container.appendChild(renderer.domElement);

        // Lighting System
        const ambientLight = new THREE.AmbientLight(0x2a0409, 1.8);
        scene.add(ambientLight);

        // Key light: warm arterial illumination from upper right
        const keyLight = new THREE.DirectionalLight(0xff4d6d, 3.2);
        keyLight.position.set(5, 6, 5);
        scene.add(keyLight);

        // Rim light: bright carmine specular rim catching the membrane edge
        const rimLight = new THREE.DirectionalLight(0xff2a55, 4.5);
        rimLight.position.set(-6, 2, -2);
        scene.add(rimLight);

        // Backlight: deep vascular wine depth
        const backLight = new THREE.DirectionalLight(0x6b0918, 3.0);
        backLight.position.set(1, -5, 3);
        scene.add(backLight);

        // -------------------------------------------------------------
        // 1. THE SWEEPING HORN/FUNNEL MEMBRANE (IntegratedBio clone)
        // -------------------------------------------------------------
        // We construct a parametric mesh with high resolution for silky curvature
        const ribbonWidthSegments = 160;
        const ribbonLengthSegments = 160;
        const ribbonGeo = new THREE.PlaneGeometry(10.0, 7.0, ribbonWidthSegments, ribbonLengthSegments);

        const ribbonMaterial = new THREE.ShaderMaterial({
            side: THREE.DoubleSide,
            transparent: true,
            uniforms: {
                uTime: { value: 0 },
                uPulse: { value: 0 },
                uMouse: { value: new THREE.Vector2(0, 0) },
                uResolution: { value: new THREE.Vector2(width, height) },
                uColorDeep: { value: new THREE.Color(0x280309) },       // Dark vascular lumen
                uColorMid: { value: new THREE.Color(0x880e1c) },        // Living tissue red
                uColorBright: { value: new THREE.Color(0xd90429) },     // Oxygenated arterial ruby
                uColorRim: { value: new THREE.Color(0xffccd5) },        // Glistening specular rim
                uColorGlow: { value: new THREE.Color(0xff2a55) }        // Bioluminescent carmine
            },
            vertexShader: `
                uniform float uTime;
                uniform float uPulse;
                uniform vec2 uMouse;
                varying vec2 vUv;
                varying vec3 vNormal;
                varying vec3 vWorldPosition;
                varying vec3 vViewPosition;
                varying float vElevation;
                varying float vRimHighlight;

                void main() {
                    vUv = uv;
                    float u = uv.x;
                    float v = uv.y;

                    // Parametric Horn/Funnel Geometry matching IntegratedBio
                    // The funnel arches across the top-right into a deep vortex
                    float angle = (v - 0.48) * 3.14159 * 1.65;
                    float r = mix(2.5, 0.85, u) + sin(u * 3.14159) * 0.45;

                    // Central spine path of the sweeping biological horn
                    float spineX = mix(-0.2, 3.2, u) + sin(u * 2.6 + uTime * 0.22) * 0.35;
                    float spineY = mix(0.4, 2.2, sin(u * 3.14159)) - (u * 1.4) + cos(u * 2.2 + uTime * 0.18) * 0.3;
                    float spineZ = mix(0.8, -3.8, u) + sin(u * 2.0) * 0.7;

                    // Cross-sectional tubular offset with rolled open membrane lip
                    vec3 offset;
                    offset.x = cos(angle) * r * 1.1;
                    offset.y = sin(angle) * r * 0.85;
                    offset.z = sin(angle * 1.8) * 0.55;

                    // Organic undulating tissue waves
                    float wave1 = sin(u * 5.0 + v * 3.5 + uTime * 0.75) * 0.18;
                    float wave2 = cos(u * 8.0 - v * 4.5 + uTime * 0.95) * 0.10;
                    
                    // Systolic arterial pulse expansion wave
                    float pulseWave = sin(u * 10.0 - uTime * 3.6) * uPulse * 0.25;

                    // Interactive mouse displacement ripple
                    vec3 basePos = vec3(spineX + offset.x, spineY + offset.y, spineZ + offset.z);
                    float mouseDist = length(vec2(basePos.x * 0.2 - uMouse.x, basePos.y * 0.2 - uMouse.y));
                    float mouseRipple = sin(mouseDist * 8.0 - uTime * 2.0) * exp(-mouseDist * 1.8) * 0.16;

                    vec3 pos = basePos;
                    pos.y += wave1 + pulseWave + (uMouse.y * 0.35);
                    pos.z += wave2 + mouseRipple + (uMouse.x * 0.4);

                    vElevation = wave1 + pulseWave + offset.z;
                    vRimHighlight = smoothstep(0.0, 0.05, v) * smoothstep(1.0, 0.95, v);

                    // Smooth surface normal calculation
                    vec3 displacedNormal = normal;
                    displacedNormal.x += cos(angle) * 0.4;
                    displacedNormal.y += sin(angle) * 0.4;
                    vNormal = normalize(normalMatrix * displacedNormal);

                    vec4 worldPos = modelMatrix * vec4(pos, 1.0);
                    vWorldPosition = worldPos.xyz;

                    vec4 mvPosition = modelViewMatrix * vec4(pos, 1.0);
                    vViewPosition = -mvPosition.xyz;
                    gl_Position = projectionMatrix * mvPosition;
                }
            `,
            fragmentShader: `
                uniform float uTime;
                uniform float uPulse;
                uniform vec3 uColorDeep;
                uniform vec3 uColorMid;
                uniform vec3 uColorBright;
                uniform vec3 uColorRim;
                uniform vec3 uColorGlow;
                varying vec2 vUv;
                varying vec3 vNormal;
                varying vec3 vWorldPosition;
                varying vec3 vViewPosition;
                varying float vElevation;
                varying float vRimHighlight;

                void main() {
                    vec3 normal = normalize(vNormal);
                    vec3 viewDir = normalize(vViewPosition);

                    // Razor-sharp specular Fresnel rim (IntegratedBio signature look)
                    float NdotV = max(0.0, dot(normal, viewDir));
                    float fresnel = pow(1.0 - NdotV, 3.8);
                    float edgeFresnel = pow(1.0 - NdotV, 1.4);

                    // Laminar micro-flow striations
                    float striations = sin(vUv.x * 160.0 + uTime * 1.8) * 0.04
                                     + sin(vUv.y * 110.0 - uTime * 1.4) * 0.02;

                    // Deep lumen to outer tissue gradient
                    float depthFactor = smoothstep(-1.2, 0.9, vElevation);
                    vec3 baseColor = mix(uColorDeep, uColorMid, depthFactor);
                    baseColor = mix(baseColor, uColorBright, smoothstep(0.15, 0.9, depthFactor + striations));

                    // Arterial systolic flash
                    baseColor += uColorGlow * (uPulse * 0.35);

                    // Subsurface scattering translucency
                    vec3 lightDir = normalize(vec3(0.5, 0.8, 0.6));
                    float sss = pow(max(0.0, dot(-viewDir, lightDir)), 2.8) * 0.38;
                    baseColor += uColorBright * sss;

                    // Gleaming specular edge highlight along the rim
                    vec3 finalColor = mix(baseColor, uColorRim, fresnel * 0.9);
                    finalColor += uColorGlow * edgeFresnel * 0.3;

                    // Alpha feathered boundary for soft biological fade
                    float alphaEdge = smoothstep(0.0, 0.12, vUv.x) * smoothstep(1.0, 0.88, vUv.x)
                                    * smoothstep(0.0, 0.08, vUv.y) * smoothstep(1.0, 0.92, vUv.y);

                    // Keep text area clear by feathering the left side
                    float textSafeFade = smoothstep(0.0, 0.15, vUv.x);
                    float finalAlpha = (0.75 + fresnel * 0.25) * alphaEdge * textSafeFade;

                    gl_FragColor = vec4(finalColor, finalAlpha);
                }
            `
        });

        const ribbonMesh = new THREE.Mesh(ribbonGeo, ribbonMaterial);
        ribbonMesh.position.set(0.8, -0.1, -0.8);
        ribbonMesh.rotation.set(-0.1, -0.22, 0.05);
        scene.add(ribbonMesh);

        // Secondary counter-sweeping ribbon for layered depth
        const secondaryGeo = new THREE.PlaneGeometry(7.0, 5.0, 110, 110);
        const secondaryMaterial = ribbonMaterial.clone();
        secondaryMaterial.uniforms = {
            uTime: { value: 0 },
            uPulse: { value: 0 },
            uMouse: { value: new THREE.Vector2(0, 0) },
            uResolution: { value: new THREE.Vector2(width, height) },
            uColorDeep: { value: new THREE.Color(0x1a0205) },
            uColorMid: { value: new THREE.Color(0x6b0918) },
            uColorBright: { value: new THREE.Color(0xb5179e) },
            uColorRim: { value: new THREE.Color(0xffccd5) },
            uColorGlow: { value: new THREE.Color(0xe11d48) }
        };
        const secondaryMesh = new THREE.Mesh(secondaryGeo, secondaryMaterial);
        secondaryMesh.position.set(-1.8, -0.8, -2.6);
        secondaryMesh.rotation.set(0.25, 0.35, -0.15);
        scene.add(secondaryMesh);

        // -------------------------------------------------------------
        // 2. ANATOMICALLY ACCURATE ERYTHROCYTES (Red Blood Cells)
        // -------------------------------------------------------------
        // Evans-Fung biconcave disc equation via LatheGeometry
        function createErythrocyteGeometry() {
            const points = [];
            const R = 0.38;
            const D = 0.24;
            const segments = 26;

            for (let i = 0; i <= segments; i++) {
                const r = (i / segments) * R;
                const rNorm = r / R;
                const sqrtTerm = Math.sqrt(Math.max(0.001, 1 - rNorm * rNorm));
                const polyTerm = 0.16 + 1.38 * (rNorm * rNorm) - 0.34 * Math.pow(rNorm, 4);
                const z = 0.5 * D * sqrtTerm * polyTerm;
                points.push(new THREE.Vector2(r, z));
            }
            for (let i = segments; i >= 0; i--) {
                const r = (i / segments) * R;
                const rNorm = r / R;
                const sqrtTerm = Math.sqrt(Math.max(0.001, 1 - rNorm * rNorm));
                const polyTerm = 0.16 + 1.38 * (rNorm * rNorm) - 0.34 * Math.pow(rNorm, 4);
                const z = -0.5 * D * sqrtTerm * polyTerm;
                points.push(new THREE.Vector2(r, z));
            }

            const geo = new THREE.LatheGeometry(points, 32);
            geo.computeVertexNormals();
            return geo;
        }

        const erythrocyteGeo = createErythrocyteGeometry();
        const erythrocyteMaterial = new THREE.MeshPhysicalMaterial({
            color: 0xa4133c,
            emissive: 0x480410,
            emissiveIntensity: 0.35,
            roughness: 0.24,
            metalness: 0.08,
            clearcoat: 0.9,
            clearcoatRoughness: 0.12,
            transmission: 0.32,
            ior: 1.4,
            specularColor: new THREE.Color(0xff8da1),
            specularIntensity: 1.3
        });

        const cellCount = config.cellCount;
        const instancedErythrocytes = new THREE.InstancedMesh(erythrocyteGeo, erythrocyteMaterial, cellCount);
        scene.add(instancedErythrocytes);

        const cellData = [];
        const dummy = new THREE.Object3D();

        for (let i = 0; i < cellCount; i++) {
            const isHeroCell = i < 5; // A few prominent foreground cells near the right edge
            cellData.push({
                progress: Math.random(),
                speed: 0.035 + Math.random() * 0.045,
                radiusX: isHeroCell ? 3.0 + Math.random() * 1.5 : 2.0 + Math.random() * 3.2,
                radiusY: isHeroCell ? 1.6 + Math.random() * 1.0 : 1.2 + Math.random() * 2.0,
                radiusZ: isHeroCell ? 1.2 + Math.random() * 1.0 : 1.8 + Math.random() * 2.8,
                offsetX: isHeroCell ? 1.5 + Math.random() * 1.5 : (Math.random() - 0.3) * 2.0,
                offsetY: (Math.random() - 0.5) * 1.5,
                offsetZ: isHeroCell ? 1.0 + Math.random() * 1.5 : (Math.random() - 0.5) * 2.5,
                rotX: Math.random() * Math.PI * 2,
                rotY: Math.random() * Math.PI * 2,
                rotZ: Math.random() * Math.PI * 2,
                rotSpeedX: (Math.random() - 0.5) * 0.7,
                rotSpeedY: (Math.random() - 0.5) * 0.8,
                rotSpeedZ: (Math.random() - 0.5) * 0.5,
                scale: isHeroCell ? 0.85 + Math.random() * 0.45 : 0.45 + Math.random() * 0.55
            });
        }

        // -------------------------------------------------------------
        // 3. GLOWING PLASMA PARTICLES & PLATELETS
        // -------------------------------------------------------------
        const particleCount = config.particleCount;
        const particleGeo = new THREE.BufferGeometry();
        const particlePositions = new Float32Array(particleCount * 3);
        const particleScales = new Float32Array(particleCount);

        for (let i = 0; i < particleCount; i++) {
            particlePositions[i * 3 + 0] = (Math.random() - 0.4) * 13;
            particlePositions[i * 3 + 1] = (Math.random() - 0.5) * 9;
            particlePositions[i * 3 + 2] = (Math.random() - 0.5) * 7 - 0.5;
            particleScales[i] = 1.0 + Math.random() * 2.8;
        }

        particleGeo.setAttribute('position', new THREE.BufferAttribute(particlePositions, 3));
        particleGeo.setAttribute('aScale', new THREE.BufferAttribute(particleScales, 1));

        const particleMaterial = new THREE.ShaderMaterial({
            transparent: true,
            blending: THREE.AdditiveBlending,
            depthWrite: false,
            uniforms: {
                uTime: { value: 0 },
                uPulse: { value: 0 },
                uColor: { value: new THREE.Color(0xff4d6d) }
            },
            vertexShader: `
                attribute float aScale;
                uniform float uTime;
                uniform float uPulse;
                varying float vAlpha;

                void main() {
                    vec3 pos = position;
                    pos.x += sin(uTime * 0.35 + pos.y * 0.4) * 0.35;
                    pos.y += cos(uTime * 0.28 + pos.x * 0.3) * 0.25;
                    
                    vec4 mvPosition = modelViewMatrix * vec4(pos, 1.0);
                    gl_PointSize = aScale * (16.0 / -mvPosition.z) * (1.0 + uPulse * 0.45);
                    gl_Position = projectionMatrix * mvPosition;
                    vAlpha = 0.35 + sin(uTime * 2.2 + pos.x) * 0.2 + uPulse * 0.3;
                }
            `,
            fragmentShader: `
                uniform vec3 uColor;
                varying float vAlpha;

                void main() {
                    vec2 uv = gl_PointCoord - vec2(0.5);
                    float dist = length(uv);
                    if (dist > 0.5) discard;
                    float glow = exp(-dist * 5.5);
                    gl_FragColor = vec4(uColor, glow * vAlpha);
                }
            `
        });

        const particleSystem = new THREE.Points(particleGeo, particleMaterial);
        scene.add(particleSystem);

        // -------------------------------------------------------------
        // 4. MOUSE PARALLAX & TILT INTERACTIVITY
        // -------------------------------------------------------------
        const mouse = { x: 0, y: 0, targetX: 0, targetY: 0 };
        const windowHalfX = window.innerWidth / 2;
        const windowHalfY = window.innerHeight / 2;

        window.addEventListener('mousemove', (e) => {
            mouse.targetX = (e.clientX - windowHalfX) / windowHalfX;
            mouse.targetY = -(e.clientY - windowHalfY) / windowHalfY;
        }, { passive: true });

        window.addEventListener('touchmove', (e) => {
            if (e.touches.length > 0) {
                mouse.targetX = (e.touches[0].clientX - windowHalfX) / windowHalfX;
                mouse.targetY = -(e.touches[0].clientY - windowHalfY) / windowHalfY;
            }
        }, { passive: true });

        // Scroll Parallax Tracking
        let scrollY = 0;
        let targetScrollY = 0;
        window.addEventListener('scroll', () => {
            targetScrollY = window.scrollY || window.pageYOffset;
        }, { passive: true });

        function onWindowResize() {
            width = window.innerWidth;
            height = window.innerHeight;
            camera.aspect = width / height;
            camera.updateProjectionMatrix();
            renderer.setSize(width, height);
            renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
            ribbonMaterial.uniforms.uResolution.value.set(width, height);
            secondaryMaterial.uniforms.uResolution.value.set(width, height);
        }
        window.addEventListener('resize', onWindowResize);

        // -------------------------------------------------------------
        // 5. ANIMATION LOOP & SYSTOLIC HEARTBEAT DYNAMICS
        // -------------------------------------------------------------
        const clock = new THREE.Clock();
        let lastBeatTime = 0;

        function animate() {
            requestAnimationFrame(animate);

            const elapsedTime = clock.getElapsedTime();

            // Spring interpolation for silky mouse response
            mouse.x += (mouse.targetX - mouse.x) * 0.045;
            mouse.y += (mouse.targetY - mouse.y) * 0.045;

            // Smooth scroll parallax progression
            scrollY += (targetScrollY - scrollY) * 0.08;
            const scrollProgress = scrollY / 1200.0;

            // Camera gracefully glides through the vascular flow as user scrolls down
            camera.position.x = 0.2 + mouse.x * 0.65;
            camera.position.y = 0.3 + mouse.y * 0.45 - scrollProgress * 0.35;
            camera.position.z = 7.5 - Math.min(scrollProgress * 0.6, 1.5);
            camera.lookAt(0.2 + mouse.x * 0.1, 0.2 + mouse.y * 0.08 - scrollProgress * 0.2, -1.0);

            // Double-peak arterial systolic pulse calculation (P-wave & dicrotic notch)
            const beatPeriod = 60.0 / config.bpm;
            const cyclePhase = (elapsedTime % beatPeriod) / beatPeriod;
            
            const p1 = Math.exp(-Math.pow(cyclePhase - 0.16, 2) * 240.0);
            const p2 = Math.exp(-Math.pow(cyclePhase - 0.30, 2) * 380.0) * 0.4;
            const pulseValue = (p1 + p2) * config.systoleIntensity;

            // Trigger synthesized heartbeat sound at cycle peak
            if (cyclePhase > 0.14 && cyclePhase < 0.20 && (elapsedTime - lastBeatTime) > (beatPeriod * 0.8)) {
                lastBeatTime = elapsedTime;
                playHeartbeatSound();
            }

            // Update uniforms
            ribbonMaterial.uniforms.uTime.value = elapsedTime * config.flowSpeed;
            ribbonMaterial.uniforms.uPulse.value = pulseValue;
            ribbonMaterial.uniforms.uMouse.value.set(mouse.x, mouse.y);

            secondaryMaterial.uniforms.uTime.value = elapsedTime * config.flowSpeed * 0.88 + 1.2;
            secondaryMaterial.uniforms.uPulse.value = pulseValue;
            secondaryMaterial.uniforms.uMouse.value.set(mouse.x * 0.7, mouse.y * 0.7);

            particleMaterial.uniforms.uTime.value = elapsedTime;
            particleMaterial.uniforms.uPulse.value = pulseValue;

            // Subtle rotation & scroll tilt of the biological ribbons
            ribbonMesh.rotation.x = -0.1 + scrollProgress * 0.12;
            ribbonMesh.rotation.z = 0.05 + Math.sin(elapsedTime * 0.18) * 0.03 + mouse.x * 0.05 + scrollProgress * 0.06;
            ribbonMesh.rotation.y = -0.22 + Math.cos(elapsedTime * 0.14) * 0.04 + mouse.x * 0.1 - scrollProgress * 0.08;

            secondaryMesh.rotation.x = 0.25 - scrollProgress * 0.1;
            secondaryMesh.rotation.z = -0.15 + Math.cos(elapsedTime * 0.2) * 0.04 - mouse.x * 0.04 - scrollProgress * 0.05;
            secondaryMesh.rotation.y = 0.35 + Math.sin(elapsedTime * 0.16) * 0.05 - mouse.x * 0.07 + scrollProgress * 0.06;

            // Update Erythrocyte positions along flow streamlines
            for (let i = 0; i < cellCount; i++) {
                const cell = cellData[i];
                cell.progress += (cell.speed * (1.0 + pulseValue * 0.75)) * 0.007;
                if (cell.progress > 1.0) cell.progress -= 1.0;

                const angle = cell.progress * Math.PI * 2;
                const x = Math.cos(angle) * cell.radiusX + cell.offsetX + Math.sin(elapsedTime * 0.35 + i) * 0.18;
                const y = Math.sin(angle) * cell.radiusY + cell.offsetY + Math.cos(elapsedTime * 0.28 + i) * 0.12;
                const z = Math.sin(angle * 2.0) * cell.radiusZ + cell.offsetZ;

                dummy.position.set(x, y, z);

                // 3D cell tumbling in laminar blood flow
                cell.rotX += cell.rotSpeedX * 0.018;
                cell.rotY += cell.rotSpeedY * 0.018;
                cell.rotZ += cell.rotSpeedZ * 0.015;
                dummy.rotation.set(cell.rotX, cell.rotY, cell.rotZ);

                // Pulse dilation
                const scale = cell.scale * (1.0 + pulseValue * 0.07);
                dummy.scale.set(scale, scale, scale);

                dummy.updateMatrix();
                instancedErythrocytes.setMatrixAt(i, dummy.matrix);
            }
            instancedErythrocytes.instanceMatrix.needsUpdate = true;

            // Render 3D scene
            renderer.render(scene, camera);
        }

        animate();

        // -------------------------------------------------------------
        // 6. TELEMETRY BADGE & AUDIO/BPM CONTROLLER
        // -------------------------------------------------------------
        setupTelemetryBadge(config);
    }

    function setupTelemetryBadge(config) {
        const badge = document.getElementById('live-bg-telemetry');
        if (!badge) return;

        const bpmLabel = badge.querySelector('.telemetry-bpm');
        const soundBtn = document.getElementById('telemetry-sound-toggle');

        const modes = [
            { label: 'Normal Arterial Flow', bpm: 72 },
            { label: 'Elevated Hemodynamics', bpm: 98 },
            { label: 'Resting Cardiac Flow', bpm: 58 }
        ];
        let currentMode = 0;

        // Click badge to cycle BPM modes
        badge.addEventListener('click', (e) => {
            if (e.target.closest('#telemetry-sound-toggle')) return;
            currentMode = (currentMode + 1) % modes.length;
            const mode = modes[currentMode];
            config.bpm = mode.bpm;
            if (bpmLabel) {
                bpmLabel.textContent = `${mode.bpm} BPM • ${mode.label}`;
            }
            badge.classList.add('pulse-pop');
            setTimeout(() => badge.classList.remove('pulse-pop'), 400);
        });

        // Click speaker button to toggle authentic heartbeat sound
        if (soundBtn) {
            soundBtn.addEventListener('click', (e) => {
                e.stopPropagation();
                isAudioMuted = !isAudioMuted;
                soundBtn.textContent = isAudioMuted ? '🔇' : '🔊';
                soundBtn.title = isAudioMuted ? 'Sound muted (click to enable heartbeat sound)' : 'Heartbeat sound active';
                if (!isAudioMuted) {
                    playHeartbeatSound();
                }
            });
        }
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initWhenReady);
    } else {
        initWhenReady();
    }
})();

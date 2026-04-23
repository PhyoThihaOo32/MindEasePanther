// Scroll reveal
const observer = new IntersectionObserver((entries) => {
  entries.forEach((entry) => {
    if (entry.isIntersecting) {
      entry.target.classList.add('visible');
      observer.unobserve(entry.target);
    }
  });
}, { threshold: 0.12, rootMargin: '0px 0px -40px 0px' });

document.querySelectorAll('.reveal').forEach((element) => observer.observe(element));

// Nav shadow on scroll
const nav = document.getElementById('nav');
window.addEventListener('scroll', () => {
  nav.classList.toggle('scrolled', window.scrollY > 20);
}, { passive: true });

const bambooFall = document.getElementById('bamboo-fall');
const prefersReducedMotion = window.matchMedia('(prefers-reduced-motion: reduce)');
let leafResizeTimer;

function randomBetween(min, max) {
  return min + Math.random() * (max - min);
}

function buildLeaves() {
  if (!bambooFall) {
    return;
  }

  bambooFall.innerHTML = '';

  if (prefersReducedMotion.matches) {
    return;
  }

  const leafCount = window.innerWidth < 700 ? 10 : 18;

  for (let index = 0; index < leafCount; index += 1) {
    const leaf = document.createElement('span');
    leaf.className = 'bamboo-leaf';

    leaf.style.setProperty('--x', `${randomBetween(0, 100).toFixed(2)}%`);
    leaf.style.setProperty('--leaf-size', `${randomBetween(18, 34).toFixed(2)}px`);
    leaf.style.setProperty('--dur', `${randomBetween(24, 42).toFixed(2)}s`);
    leaf.style.setProperty('--delay', `${randomBetween(-42, 0).toFixed(2)}s`);
    leaf.style.setProperty('--tilt', `${randomBetween(-26, 26).toFixed(2)}deg`);
    leaf.style.setProperty('--scale', randomBetween(0.72, 1.36).toFixed(2));
    leaf.style.setProperty('--opacity', randomBetween(0.22, 0.48).toFixed(2));
    leaf.style.setProperty('--drift-a', `${randomBetween(-24, 28).toFixed(2)}px`);
    leaf.style.setProperty('--drift-b', `${randomBetween(-34, 30).toFixed(2)}px`);
    leaf.style.setProperty('--drift-c', `${randomBetween(-28, 24).toFixed(2)}px`);
    leaf.style.setProperty('--drift-d', `${randomBetween(-20, 20).toFixed(2)}px`);

    bambooFall.appendChild(leaf);
  }
}

buildLeaves();

window.addEventListener('resize', () => {
  window.clearTimeout(leafResizeTimer);
  leafResizeTimer = window.setTimeout(buildLeaves, 180);
});

prefersReducedMotion.addEventListener('change', buildLeaves);

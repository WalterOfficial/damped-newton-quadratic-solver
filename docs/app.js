document.addEventListener("DOMContentLoaded", () => {
  if (window.renderMathInElement) {
    window.renderMathInElement(document.body, {
      delimiters: [
        { left: "$$", right: "$$", display: true },
        { left: "\\(", right: "\\)", display: false },
        { left: "\\[", right: "\\]", display: true },
        { left: "$", right: "$", display: false }
      ],
      throwOnError: false
    });
  }

  const revealTargets = [
    ...document.querySelectorAll(".hero-copy, .hero-panel, .section, .callout, .info-card, .math-card, .timeline article, .graph-card")
  ];

  revealTargets.forEach((element, index) => {
    element.classList.add("reveal", `reveal-delay-${index % 4}`);
  });

  const observer = new IntersectionObserver((entries) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        entry.target.classList.add("is-visible");
        observer.unobserve(entry.target);
      }
    });
  }, { threshold: 0.12 });

  revealTargets.forEach((element) => observer.observe(element));
});

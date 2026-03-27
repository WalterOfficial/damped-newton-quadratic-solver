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

  const revealItems = [...document.querySelectorAll(".reveal")];
  const observer = new IntersectionObserver((entries) => {
    entries.forEach((entry) => {
      if (!entry.isIntersecting) {
        return;
      }
      entry.target.classList.add("is-visible");
      observer.unobserve(entry.target);
    });
  }, { threshold: 0.14 });

  revealItems.forEach((item) => observer.observe(item));

  const meterObserver = new IntersectionObserver((entries) => {
    entries.forEach((entry) => {
      if (!entry.isIntersecting) {
        return;
      }
      const fill = entry.target.dataset.fill || "0";
      entry.target.style.width = `${fill}%`;
      meterObserver.unobserve(entry.target);
    });
  }, { threshold: 0.5 });

  document.querySelectorAll(".meter-track span").forEach((bar) => {
    meterObserver.observe(bar);
  });
});

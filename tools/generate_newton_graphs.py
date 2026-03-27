import math
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
ASSETS = ROOT / "assets"
ASSETS.mkdir(exist_ok=True)

MAX_ITERS = 25
EPS = 1e-12


def pure_newton(f, df, x0):
    points = []
    x = x0
    for i in range(MAX_ITERS):
        fx = f(x)
        points.append((i, x, fx))
        if abs(fx) < 1e-12:
            return True, points
        d = df(x)
        if abs(d) < EPS:
            return False, points
        x = x - fx / d
        if not math.isfinite(x):
            return False, points
    return abs(f(x)) < 1e-10, points


def damped_newton(f, df, x0):
    points = []
    x = x0
    for i in range(MAX_ITERS):
        fx = f(x)
        points.append((i, x, fx))
        if abs(fx) < 1e-12:
            return True, points
        dphi = df(x) / (1.0 + fx * fx)
        if abs(dphi) < EPS:
            return False, points
        delta = math.atan(fx) / dphi
        ad = abs(delta)
        lam = 1.0 if ad < 1e-15 else math.atan(ad) / ad
        x = x - lam * delta
        if not math.isfinite(x):
            return False, points
    return abs(f(x)) < 1e-10, points


def residual_series(points):
    out = []
    for i, _, fx in points:
        out.append((i, math.log10(abs(fx) + 1e-16)))
    return out


def polyline(series, x_min, x_max, y_min, y_max, width, height, pad):
    coords = []
    for x, y in series:
        px = pad + (x - x_min) / (x_max - x_min) * (width - 2 * pad)
        py = height - pad - (y - y_min) / (y_max - y_min) * (height - 2 * pad)
        coords.append(f"{px:.1f},{py:.1f}")
    return " ".join(coords)


def make_svg(title, pure_points, damped_points, filename):
    pure_series = residual_series(pure_points)
    damped_series = residual_series(damped_points)
    all_series = pure_series + damped_series

    x_min = 0
    x_max = max(p[0] for p in all_series) if all_series else 1
    y_min = min(p[1] for p in all_series) - 0.5
    y_max = max(p[1] for p in all_series) + 0.5
    if abs(y_max - y_min) < 1e-9:
        y_max = y_min + 1.0

    width, height, pad = 900, 420, 60
    grid = []
    for i in range(6):
        y = pad + i * (height - 2 * pad) / 5
        value = y_max - i * (y_max - y_min) / 5
        grid.append(
            f'<line x1="{pad}" y1="{y:.1f}" x2="{width-pad}" y2="{y:.1f}" stroke="#273244" stroke-width="1"/>'
            f'<text x="12" y="{y+4:.1f}" fill="#9fb0c7" font-size="14">{value:.2f}</text>'
        )
    for i in range(x_max + 1):
        x = pad + (0 if x_max == 0 else i * (width - 2 * pad) / x_max)
        grid.append(
            f'<line x1="{x:.1f}" y1="{pad}" x2="{x:.1f}" y2="{height-pad}" stroke="#1b2330" stroke-width="1"/>'
            f'<text x="{x-4:.1f}" y="{height-20}" fill="#9fb0c7" font-size="14">{i}</text>'
        )

    pure_poly = polyline(pure_series, x_min, max(1, x_max), y_min, y_max, width, height, pad)
    damped_poly = polyline(damped_series, x_min, max(1, x_max), y_min, y_max, width, height, pad)

    svg = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">
  <rect width="100%" height="100%" fill="#0d1117"/>
  <text x="{pad}" y="30" fill="#f0f6fc" font-size="24" font-family="Segoe UI, Arial, sans-serif">{title}</text>
  <text x="{pad}" y="52" fill="#9fb0c7" font-size="14" font-family="Segoe UI, Arial, sans-serif">По оси Y: log10(|f(x_n)|), по оси X: номер итерации</text>
  {''.join(grid)}
  <line x1="{pad}" y1="{height-pad}" x2="{width-pad}" y2="{height-pad}" stroke="#8b949e" stroke-width="2"/>
  <line x1="{pad}" y1="{pad}" x2="{pad}" y2="{height-pad}" stroke="#8b949e" stroke-width="2"/>
  <polyline fill="none" stroke="#58a6ff" stroke-width="3" points="{pure_poly}"/>
  <polyline fill="none" stroke="#3fb950" stroke-width="3" points="{damped_poly}"/>
  <circle cx="{width-270}" cy="34" r="6" fill="#58a6ff"/>
  <text x="{width-255}" y="39" fill="#c9d1d9" font-size="15" font-family="Segoe UI, Arial, sans-serif">Чистый Ньютон</text>
  <circle cx="{width-270}" cy="60" r="6" fill="#3fb950"/>
  <text x="{width-255}" y="65" fill="#c9d1d9" font-size="15" font-family="Segoe UI, Arial, sans-serif">Демпфированный Ньютон</text>
</svg>
"""
    (ASSETS / filename).write_text(svg, encoding="utf-8")


CASES = [
    (
        "Сложный старт: cos(x) - x, x0 = -5.5",
        lambda x: math.cos(x) - x,
        lambda x: -math.sin(x) - 1,
        -5.5,
        "complex_cos_minus_x.svg",
    ),
    (
        "Сложный старт: x - sin(x) - 0.5, x0 = -2.5",
        lambda x: x - math.sin(x) - 0.5,
        lambda x: 1 - math.cos(x),
        -2.5,
        "complex_x_minus_sin.svg",
    ),
    (
        "Сложный старт: atan(x) - 0.8, x0 = -6.0",
        lambda x: math.atan(x) - 0.8,
        lambda x: 1 / (1 + x * x),
        -6.0,
        "complex_atan_minus_const.svg",
    ),
]


for title, f, df, x0, filename in CASES:
    _, pure_points = pure_newton(f, df, x0)
    _, damped_points = damped_newton(f, df, x0)
    make_svg(title, pure_points, damped_points, filename)


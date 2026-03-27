# Damped Newton Quadratic Solver

A compact C++ console program for finding the real roots of

```text
a*x^2 + b*x + c = 0
```

using:

- extremum analysis of the parabola
- a damped Newton method
- arctangent-based stabilization

## Overview

The program reads the coefficients `a`, `b`, and `c`, determines how many real roots exist, and then refines those roots numerically.

Instead of using only the discriminant formula, the solver first analyzes the extremum of the quadratic function:

```text
f(x) = a*x^2 + b*x + c
```

The extremum is located at:

```text
x* = -b / (2a)
```

The value `f(x*)` tells us the real-root structure:

- `f(x*) = 0` -> one double root
- upward parabola with `f(x*) > 0` -> no real roots
- upward parabola with `f(x*) < 0` -> two real roots
- downward parabola works symmetrically

After that, each real root is refined with a damped Newton iteration.

## Why `atan` is used

The solver does not apply Newton directly to `f(x)`. Instead, it uses:

```text
phi(x) = atan(s * f(x))
```

where `s` is a scaling factor.

This helps because `atan(...)` compresses large values of the function and makes the iteration more stable when the starting point is far from the root.

The derivative is:

```text
phi'(x) = s * f'(x) / (1 + (s*f(x))^2)
```

with

```text
f'(x) = 2*a*x + b
```

## Damping rule

The raw Newton step is:

```text
delta = phi(x) / phi'(x)
```

Then the program applies damping:

```text
lambda = atan(|delta|) / |delta|
```

and updates the approximation as:

```text
x_(k+1) = x_k - lambda * delta
```

This means:

- small steps behave almost like ordinary Newton
- large steps are automatically reduced

## Root search strategy

If two real roots exist, the extremum splits the parabola into the left and right monotonic branches.

The program expands outward from the extremum until it finds a sign change on each side, then uses the midpoint of that interval as the initial approximation for the damped Newton method.

## Special cases

- if `a = 0`, the program solves the linear equation `b*x + c = 0`
- if `a = 0`, `b = 0`, and `c = 0`, there are infinitely many solutions
- if `a = 0`, `b = 0`, and `c != 0`, there are no solutions
- only real roots are reported

## Project structure

```text
.
в”њв”Ђ quadratic_newton.cpp
в”њв”Ђ quadratic_newton.exe
в””в”Ђ README.md
```

## Build

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic quadratic_newton.cpp -o quadratic_newton
```

On Windows:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic quadratic_newton.cpp -o quadratic_newton.exe
```

## Run

```bash
./quadratic_newton
```

or on Windows:

```bash
quadratic_newton.exe
```

## Example

Input:

```text
1 -3 2
```

Output:

```text
Quadratic equation solver by damped Newton method
Form: a*x^2 + b*x + c = 0

Enter a, b, c: Extremum point: x* = 1.500000000000
f(x*) = -0.250000000000
Number of real roots: 2
x1 = 1.000000000000
x2 = 2.000000000000
Iterations for x1: 7
Iterations for x2: 7
```

## Verified cases

- `1 -3 2` -> two roots: `1` and `2`
- `1 2 1` -> one double root: `-1`
- `1 0 1` -> no real roots
- `0 2 -8` -> linear case, root `4`

## Main components

- `polynomial(...)` computes the quadratic value
- `derivative(...)` computes the derivative
- `transformed_function(...)` builds `atan(s*f(x))`
- `transformed_derivative(...)` computes its derivative
- `damping_from_step(...)` computes the damping factor
- `damped_newton(...)` runs the iterative method

## Possible improvements

- add complex root support
- add unit tests
- add a CMake build
- add repeated interactive runs without restarting the program

## Repository

GitHub: [WalterOfficial/damped-newton-quadratic-solver](https://github.com/WalterOfficial/damped-newton-quadratic-solver)

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

constexpr double kEps = 1e-12;
constexpr int kMaxIterations = 100;

struct NewtonResult {
    bool converged = false;
    double root = 0.0;
    int iterations = 0;
};

double polynomial(double a, double b, double c, double x) {
    return (a * x + b) * x + c;
}

double derivative(double a, double b, double x) {
    return 2.0 * a * x + b;
}

double scale_factor(double a, double b, double c) {
    const double scale = std::max(1.0, std::max(std::abs(a), std::max(std::abs(b), std::abs(c))));
    return 1.0 / scale;
}

double transformed_function(double a, double b, double c, double x, double scale) {
    return std::atan(scale * polynomial(a, b, c, x));
}

double transformed_derivative(double a, double b, double c, double x, double scale) {
    const double fx = polynomial(a, b, c, x);
    const double dfx = derivative(a, b, x);
    const double z = scale * fx;
    return (scale * dfx) / (1.0 + z * z);
}

double damping_from_step(double step) {
    const double abs_step = std::abs(step);
    if (abs_step < 1e-15) {
        return 1.0;
    }
    return std::atan(abs_step) / abs_step;
}

NewtonResult damped_newton(double a, double b, double c, double start_x) {
    const double scale = scale_factor(a, b, c);
    double x = start_x;

    for (int iter = 1; iter <= kMaxIterations; ++iter) {
        const double fx = polynomial(a, b, c, x);
        if (std::abs(fx) < 1e-11) {
            return {true, x, iter - 1};
        }

        const double phi = transformed_function(a, b, c, x, scale);
        const double dphi = transformed_derivative(a, b, c, x, scale);
        if (std::abs(dphi) < kEps) {
            return {false, x, iter - 1};
        }

        const double raw_step = phi / dphi;
        const double lambda = damping_from_step(raw_step);
        const double next_x = x - lambda * raw_step;

        if (std::abs(next_x - x) < 1e-12) {
            return {std::abs(polynomial(a, b, c, next_x)) < 1e-9, next_x, iter};
        }
        x = next_x;
    }

    return {std::abs(polynomial(a, b, c, x)) < 1e-9, x, kMaxIterations};
}

double initial_radius(double a, double b, double c, double x_vertex) {
    return 1.0 + std::abs(x_vertex) + std::abs(b / (a == 0.0 ? 1.0 : a)) + std::sqrt(std::abs(c) + 1.0);
}

double expand_left_until_sign_change(double a, double b, double c, double x_vertex) {
    double right = x_vertex;
    double left = x_vertex - initial_radius(a, b, c, x_vertex);
    double f_right = polynomial(a, b, c, right);
    double f_left = polynomial(a, b, c, left);

    for (int i = 0; i < 60 && f_left * f_right > 0.0; ++i) {
        left -= std::pow(2.0, i + 1);
        f_left = polynomial(a, b, c, left);
    }
    return (left + right) * 0.5;
}

double expand_right_until_sign_change(double a, double b, double c, double x_vertex) {
    double left = x_vertex;
    double right = x_vertex + initial_radius(a, b, c, x_vertex);
    double f_left = polynomial(a, b, c, left);
    double f_right = polynomial(a, b, c, right);

    for (int i = 0; i < 60 && f_left * f_right > 0.0; ++i) {
        right += std::pow(2.0, i + 1);
        f_right = polynomial(a, b, c, right);
    }
    return (left + right) * 0.5;
}

void print_result_header() {
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Quadratic equation solver by damped Newton method\n";
    std::cout << "Form: a*x^2 + b*x + c = 0\n\n";
}

}  // namespace

int main() {
    print_result_header();

    double a = 0.0;
    double b = 0.0;
    double c = 0.0;

    std::cout << "Enter a, b, c: ";
    if (!(std::cin >> a >> b >> c)) {
        std::cerr << "Input error.\n";
        return 1;
    }

    if (std::abs(a) < kEps) {
        if (std::abs(b) < kEps) {
            if (std::abs(c) < kEps) {
                std::cout << "Infinitely many solutions.\n";
            } else {
                std::cout << "No real roots.\n";
            }
            return 0;
        }

        const double x = -c / b;
        std::cout << "Linear equation detected.\n";
        std::cout << "One real root: x = " << x << "\n";
        return 0;
    }

    const double x_vertex = -b / (2.0 * a);
    const double y_vertex = polynomial(a, b, c, x_vertex);

    std::cout << "Extremum point: x* = " << x_vertex << "\n";
    std::cout << "f(x*) = " << y_vertex << "\n";

    if (std::abs(y_vertex) < 1e-11) {
        const auto result = damped_newton(a, b, c, x_vertex);
        std::cout << "Number of real roots: 1 (double root)\n";
        std::cout << "x1 = x2 = " << result.root << "\n";
        std::cout << "Iterations: " << result.iterations << "\n";
        return 0;
    }

    const bool has_two_roots = (a > 0.0 && y_vertex < 0.0) || (a < 0.0 && y_vertex > 0.0);
    if (!has_two_roots) {
        std::cout << "Number of real roots: 0\n";
        return 0;
    }

    const double left_start = expand_left_until_sign_change(a, b, c, x_vertex);
    const double right_start = expand_right_until_sign_change(a, b, c, x_vertex);

    const auto left_root = damped_newton(a, b, c, left_start);
    const auto right_root = damped_newton(a, b, c, right_start);

    if (!left_root.converged || !right_root.converged) {
        std::cerr << "Newton method did not converge for at least one root.\n";
        return 2;
    }

    std::vector<double> roots = {left_root.root, right_root.root};
    if (roots[0] > roots[1]) {
        std::swap(roots[0], roots[1]);
    }

    std::cout << "Number of real roots: 2\n";
    std::cout << "x1 = " << roots[0] << "\n";
    std::cout << "x2 = " << roots[1] << "\n";
    std::cout << "Iterations for x1: " << left_root.iterations << "\n";
    std::cout << "Iterations for x2: " << right_root.iterations << "\n";

    return 0;
}

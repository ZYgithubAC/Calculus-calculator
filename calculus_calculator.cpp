#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr double EPSILON = 1e-10;

bool isZero(double value) {
    return std::abs(value) < EPSILON;
}

std::string formatNumber(double value) {
    if (isZero(value)) {
        return "0";
    }
    if (isZero(value - std::round(value))) {
        return std::to_string(static_cast<long long>(std::llround(value)));
    }

    std::ostringstream output;
    output << std::fixed << std::setprecision(6) << value;
    std::string text = output.str();
    text.erase(text.find_last_not_of('0') + 1);
    text.erase(text.find_last_not_of('.') + 1);
    return text;
}

class Polynomial {
public:
    explicit Polynomial(std::vector<double> coefficients = {0.0})
        : coefficients_(std::move(coefficients)) {
        normalize();
    }

    int degree() const {
        return isZero() ? -1 : static_cast<int>(coefficients_.size()) - 1;
    }

    bool isZero() const {
        return coefficients_.size() == 1 && ::isZero(coefficients_.front());
    }

    std::string toString() const {
        if (isZero()) {
            return "0";
        }

        std::ostringstream output;
        const int highestPower = degree();
        bool firstTerm = true;

        for (std::size_t i = 0; i < coefficients_.size(); ++i) {
            const double coefficient = coefficients_[i];
            if (::isZero(coefficient)) {
                continue;
            }

            const int power = highestPower - static_cast<int>(i);
            const double magnitude = std::abs(coefficient);

            if (!firstTerm) {
                output << (coefficient < 0 ? " - " : " + ");
            } else if (coefficient < 0) {
                output << '-';
            }

            if (power == 0 || !::isZero(magnitude - 1.0)) {
                output << formatNumber(magnitude);
            }
            if (power > 0) {
                output << 'x';
                if (power > 1) {
                    output << '^' << power;
                }
            }
            firstTerm = false;
        }
        return output.str();
    }

    double evaluate(double x) const {
        double result = 0.0;
        for (double coefficient : coefficients_) {
            result = result * x + coefficient;
        }
        return result;
    }

    Polynomial derivative() const {
        const int highestPower = degree();
        if (highestPower <= 0) {
            return Polynomial();
        }

        std::vector<double> result;
        result.reserve(coefficients_.size() - 1);
        for (std::size_t i = 0; i + 1 < coefficients_.size(); ++i) {
            result.push_back(coefficients_[i] * (highestPower - static_cast<int>(i)));
        }
        return Polynomial(result);
    }

    Polynomial antiderivative(double constant = 0.0) const {
        if (isZero()) {
            return Polynomial({constant});
        }

        const int highestPower = degree();
        std::vector<double> result;
        result.reserve(coefficients_.size() + 1);
        for (std::size_t i = 0; i < coefficients_.size(); ++i) {
            const int newPower = highestPower - static_cast<int>(i) + 1;
            result.push_back(coefficients_[i] / newPower);
        }
        result.push_back(constant);
        return Polynomial(result);
    }

private:
    std::vector<double> coefficients_; // Highest-degree coefficient first.

    void normalize() {
        while (coefficients_.size() > 1 && ::isZero(coefficients_.front())) {
            coefficients_.erase(coefficients_.begin());
        }
        if (coefficients_.empty()) {
            coefficients_.push_back(0.0);
        }
    }
};

template <typename T>
T readNumber(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        }
        std::cout << "Please enter a valid number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

Polynomial readPolynomial(const std::string& name) {
    int degree;
    do {
        degree = readNumber<int>("Degree of " + name + " (0 or greater): ");
        if (degree < 0) {
            std::cout << "The degree cannot be negative.\n";
        }
    } while (degree < 0);

    std::vector<double> coefficients;
    coefficients.reserve(static_cast<std::size_t>(degree + 1));
    for (int power = degree; power >= 0; --power) {
        coefficients.push_back(readNumber<double>(
            "Coefficient of x^" + std::to_string(power) + ": "));
    }

    Polynomial result(coefficients);
    std::cout << name << " = " << result.toString() << "\n\n";
    return result;
}

void printMenu() {
    std::cout << "\nCalculus Calculator\n"
              << "1. Differentiate: d/dx [P(x)]\n"
              << "2. Indefinite integral: integral of P(x) dx\n"
              << "3. Definite integral: integral from a to b of P(x) dx\n"
              << "4. Evaluate a polynomial at x\n"
              << "0. Exit\n";
}

} // namespace

int main() {
    std::cout << "Calculus Calculator (.exe)\n";
    std::cout << "Enter coefficients from the highest power down to the constant.\n";

    while (true) {
        printMenu();
        const int choice = readNumber<int>("Choose an option: ");
        std::cout << '\n';

        if (choice == 0) {
            std::cout << "Goodbye!\n";
            break;
        }

        if (choice < 1 || choice > 4) {
            std::cout << "Choose a number from 0 to 4.\n";
            continue;
        }

        const Polynomial polynomial = readPolynomial("P(x)");
        if (choice == 1) {
            std::cout << "d/dx [" << polynomial.toString() << "] = "
                      << polynomial.derivative().toString() << "\n";
        } else if (choice == 2) {
            std::cout << "Integral of " << polynomial.toString() << " dx = "
                      << polynomial.antiderivative().toString() << " + C\n";
        } else if (choice == 3) {
            const double lowerBound = readNumber<double>("Lower bound a: ");
            const double upperBound = readNumber<double>("Upper bound b: ");
            const Polynomial antiderivative = polynomial.antiderivative();
            const double result = antiderivative.evaluate(upperBound) -
                                  antiderivative.evaluate(lowerBound);
            std::cout << "Integral from " << formatNumber(lowerBound) << " to "
                      << formatNumber(upperBound) << " of " << polynomial.toString()
                      << " dx = " << formatNumber(result) << "\n";
        } else {
            const double x = readNumber<double>("Value of x: ");
            std::cout << "P(" << formatNumber(x) << ") = "
                      << formatNumber(polynomial.evaluate(x)) << "\n";
        }
    }

    return 0;
}

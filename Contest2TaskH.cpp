#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstdint>

static constexpr double PI = std::acos(-1.0);

void bitReversePermutation(std::vector<std::complex<double>>& a) {
    std::size_t n = a.size();
    for (std::size_t i = 1, j = 0; i < n; ++i) {
        std::size_t bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }

        j |= bit;
        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }
}

void fft(std::vector<std::complex<double>>& a, int direction) {
    bitReversePermutation(a);
    std::size_t n = a.size();

    for (std::size_t len = 2; len <= n; len <<= 1) {
        double angle = 2 * PI / static_cast<double>(len) * direction;
        std::complex<double> wlen(std::cos(angle), std::sin(angle));

        for (std::size_t i = 0; i < n; i += len) {
            std::complex<double> w(1.0, 0.0);

            for (std::size_t j = 0; j < len / 2; ++j) {
                std::complex<double> u = a[i + j];
                std::complex<double> v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (direction == -1) {
        for (auto& x : a) {
            x /= static_cast<double>(n);
        }
    }
}

std::vector<double> readPolynomial(int& degree) {
    std::cin >> degree;
    std::vector<double> coeffs(degree + 1);

    for (int i = degree; i >= 0; --i) {
        std::cin >> coeffs[i];
    }

    return coeffs;
}

std::vector<std::int64_t> multiplyPolynomials(
    const std::vector<double>& A,
    const std::vector<double>& B
) {
    int degA = static_cast<int>(A.size()) - 1;
    int degB = static_cast<int>(B.size()) - 1;
    int resultSize = degA + degB + 1;
    int n = 1;

    while (n < resultSize) {
        n <<= 1;
    }

    std::vector<std::complex<double>> fa(n, 0.0), fb(n, 0.0);
    for (int i = 0; i <= degA; ++i) fa[i] = A[i];
    for (int i = 0; i <= degB; ++i) fb[i] = B[i];

    fft(fa, 1);
    fft(fb, 1);

    for (int i = 0; i < n; ++i) {
        fa[i] *= fb[i];
    }

    fft(fa, -1);

    std::vector<std::int64_t> result(resultSize);
    for (int i = 0; i < resultSize; ++i) {
        result[i] = static_cast<std::int64_t>(std::llround(fa[i].real()));
    }
    return result;
}

void printPolynomial(const std::vector<std::int64_t>& coeffs) {
    int degree = static_cast<int>(coeffs.size()) - 1;
    std::cout << degree << ' ';

    for (int i = degree; i >= 0; --i) {
        std::cout << coeffs[i] << ' ';
    }
}

int main() {
    int degA, degB;
    std::vector<double> polyA = readPolynomial(degA);
    std::vector<double> polyB = readPolynomial(degB);

    std::vector<std::int64_t> product = multiplyPolynomials(polyA, polyB);
    printPolynomial(product);

    return 0;
}

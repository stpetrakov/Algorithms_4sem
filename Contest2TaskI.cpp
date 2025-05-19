#include <iostream>
#include <vector>
#include <algorithm>

const int kModulus = 7340033;
const int kPrimitiveRoot = 3;

using std::cin;
using std::cout;
using std::vector;

long long ModularPower(long long base, long long exponent) {
    long long result = 1;
    while (exponent) {
        if (exponent & 1) {
            result = (result * base) % kModulus;
        }
        base = (base * base) % kModulus;
        exponent >>= 1;
    }
    return result;
}

long long ModularInverse(long long value) {
    return ModularPower(value, kModulus - 2);
}

void NTT(vector<int>& a, bool invert) {
    int n = static_cast<int>(a.size());
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;

        if (i < j)
            std::swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        int wlen = ModularPower(kPrimitiveRoot, (kModulus - 1) / len);
        if (invert) {
            wlen = ModularInverse(wlen);
        }
        for (int i = 0; i < n; i += len) {
            int w = 1;
            for (int j = 0; j < len / 2; ++j) {
                int u = a[i + j];
                int v = (int)(1LL * a[i + j + len / 2] * w % kModulus);

                a[i + j] = u + v < kModulus ? u + v : u + v - kModulus;
                a[i + j + len / 2] = u - v >= 0 ? u - v : u - v + kModulus;
                w = (int)(1LL * w * wlen % kModulus);
            }
        }
    }

    if (invert) {
        long long n_inv = ModularInverse(n);
        for (int& x : a)
            x = (int)(1LL * x * n_inv % kModulus);
    }
}

vector<int> MultiplyPolynomials(const vector<int>& a, const vector<int>& b, int limit) {
    int n = 1;
    while (n < static_cast<int>(a.size() + b.size()))
        n <<= 1;
    vector<int> fa(a.begin(), a.end()), fb(b.begin(), b.end());
    fa.resize(n);
    fb.resize(n);

    NTT(fa, false);
    NTT(fb, false);
    for (int i = 0; i < n; ++i) {
        fa[i] = (int)(1LL * fa[i] * fb[i] % kModulus);
    }
    NTT(fa, true);

    if (static_cast<int>(fa.size()) > limit) {
        fa.resize(limit);
    }
    return fa;
}

void ReadInput(int& degree_q, int& degree_p, vector<int>& coefficients_p) {
    cin >> degree_q >> degree_p;
    coefficients_p.resize(degree_p + 1);
    for (int i = 0; i <= degree_p; ++i) {
        cin >> coefficients_p[i];
    }
}

bool SolvePolynomial(int degree_q, int degree_p, const vector<int>& coefficients_p, vector<int>& coefficients_q) {
    if (coefficients_p.empty() || coefficients_p[0] == 0) {
        return false;
    }

    coefficients_q = {static_cast<int>(ModularInverse(coefficients_p[0]))}; // Начинаем с q(x) = 1 / P(0)

    int current_length = 1;
    while (current_length < degree_q) {
        int next_length = std::min(current_length * 2, degree_q);

        vector<int> p_cut(std::min(next_length, degree_p + 1), 0);
        for (int i = 0; i < static_cast<int>(p_cut.size()); ++i) {
            p_cut[i] = coefficients_p[i];
        }

        vector<int> pq = MultiplyPolynomials(p_cut, coefficients_q, next_length);

        for (int i = 0; i < static_cast<int>(pq.size()); ++i) {
            pq[i] = (kModulus - pq[i]) % kModulus;
        }
        if (!pq.empty()) {
            pq[0] = (pq[0] + 2) % kModulus;
        } else {
            pq.push_back(2 % kModulus);
        }

        vector<int> new_q = MultiplyPolynomials(coefficients_q, pq, next_length);
        new_q.resize(next_length);

        coefficients_q = new_q;
        current_length = next_length;
    }

    coefficients_q.resize(degree_q);
    return true;
}

void PrintOutput(const vector<int>& coefficients_q, bool has_solution) {
    if (!has_solution) {
        cout << "The ears of a dead donkey\n";
        return;
    }

    for (int i = 0; i < static_cast<int>(coefficients_q.size()); ++i) {
        cout << coefficients_q[i];
        if (i + 1 != static_cast<int>(coefficients_q.size())) {
            cout << " ";
        }
    }
    cout << "\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int degree_q, degree_p;
    vector<int> coefficients_p;

    ReadInput(degree_q, degree_p, coefficients_p);

    vector<int> coefficients_q;
    bool has_solution = SolvePolynomial(degree_q, degree_p, coefficients_p, coefficients_q);

    PrintOutput(coefficients_q, has_solution);

    return 0;
}

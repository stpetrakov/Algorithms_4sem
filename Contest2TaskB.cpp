#include <iostream>
#include <vector>

const int MAX_N = 1000000;

std::vector<int> compute_least_prime_divisors(int N) {
    std::vector<int> lsp(N + 1, 0);
    for (int i = 2; i <= N; ++i) {
        if (lsp[i] == 0) {
            lsp[i] = i;
            for (int j = 2 * i; j <= N; j += i) {
                if (lsp[j] == 0) {
                    lsp[j] = i;
                }
            }
        }
    }
    return lsp;
}

long long calculate_sum(const std::vector<int>& lsp, int N) {
    long long sum = 0;
    for (int i = 4; i <= N; ++i) {
        if (lsp[i] != i) {
            sum += lsp[i];
        }
    }
    return sum;
}

int main() {
    int N;
    std::cin >> N;

    if (N < 4) {
        std::cout << 0 << std::endl;
        return 0;
    }

    std::vector<int> lsp = compute_least_prime_divisors(N);
    long long result = calculate_sum(lsp, N);

    std::cout << result;
    
    return 0;
}
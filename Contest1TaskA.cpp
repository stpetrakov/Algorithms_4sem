#include <iostream>
#include <string>
#include <vector>

std::vector<size_t> prefix_function(const std::string &s) {
    size_t n = s.size();
    std::vector<size_t> pi(n);
    for (size_t i = 1; i < n; ++i) {
        size_t j = pi[i - 1];
        while (j > 0 && s[i] != s[j]) {
            j = pi[j - 1];
        }
        if (s[i] == s[j]) {
            ++j;
        }
        pi[i] = j;
    }
    return pi;
}

void solve(const std::string &pattern, const std::string &text) {
    size_t m = pattern.size();
    auto pi = prefix_function(pattern);
    size_t j = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j]) {
            j = pi[j - 1];
        }
        if (text[i] == pattern[j]) {
            ++j;
        }
        if (j == m) {
            std::cout << (i - m + 1) << "\n";
            j = pi[j - 1];
        }
    }
}

int main() {
    std::string str_a, str_b;
    std::cin >> str_a >> str_b;

    solve(str_b, str_a);
    return 0;
}

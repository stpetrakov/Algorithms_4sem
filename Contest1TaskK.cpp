#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

inline bool eq_pat(char pa, char pb) {
    return pa == '?' || pa == pb;
}

std::vector<int> build_next(const std::string &P) {
    int m = P.size();
    std::vector<int> nxt(m);
    nxt[0] = m;
    int l = 0, r = 0;
    for (int i = 1; i < m; i++) {
        int k = 0;
        if (i <= r)
            k = std::min(nxt[i - l], r - i + 1);
        while (i + k < m && eq_pat(P[k], P[i + k]))
            k++;
        nxt[i] = k;
        if (i + k - 1 > r) {
            l = i;
            r = i + k - 1;
        }
    }
    return nxt;
}

std::vector<int> build_ext(const std::string &P, const std::string &T, const std::vector<int> &nxt) {
    int m = P.size(), n = T.size();
    std::vector<int> ext(n);
    int l = 0, r = -1;
    for (int i = 0; i < n; i++) {
        int k = 0;
        if (i <= r) {
            k = std::min(nxt[i - l], r - i + 1);
        }
        while (k < m && i + k < n && eq_pat(P[k], T[i + k]))
            k++;
        ext[i] = k;
        if (i + k - 1 > r) {
            l = i;
            r = i + k - 1;
        }
    }
    return ext;
}

std::vector<int> find_matches(const std::string &P, const std::string &T) {
    int m = P.size(), n = T.size();
    std::vector<int> result;

    if (std::count(P.begin(), P.end(), '?') == m) {
        result.reserve(n - m + 1);
        for (int i = 0; i + m <= n; i++) {
            result.push_back(i);
        }
        return result;
    }

    auto nxtF = build_next(P);
    auto extF = build_ext(P, T, nxtF);

    std::string Pr = P; std::reverse(Pr.begin(), Pr.end());
    std::string Tr = T; std::reverse(Tr.begin(), Tr.end());
    auto nxtRrev = build_next(Pr);
    auto extRrev = build_ext(Pr, Tr, nxtRrev);

    std::vector<int> extR(n);
    for (int i = 0; i < n; i++) {
        extR[n - 1 - i] = extRrev[i];
    }

    for (int i = 0; i + m <= n; i++) {
        if (extF[i] + extR[i + m - 1] >= m) {
            result.push_back(i);
        }
    }
    return result;
}

void print_positions(const std::vector<int> &pos) {
    for (size_t j = 0; j < pos.size(); j++) {
        if (j) std::cout << ' ';
        std::cout << pos[j];
    }
    std::cout << '\n';
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string P, T;
    std::getline(std::cin, P);
    std::getline(std::cin, T);

    auto positions = find_matches(P, T);
    print_positions(positions);

    return 0;
}

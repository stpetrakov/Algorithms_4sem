#include <iostream>
#include <vector>
#include <string>

bool isMatch(const std::string& text, int pos, const std::string& pattern) {
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] != '?' && text[pos + i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

std::vector<int> findMatches(const std::string& pattern, const std::string& text) {
    std::vector<int> positions;
    int m = pattern.size();
    int n = text.size();
    for (int i = 0; i <= n - m; ++i) {
        if (isMatch(text, i, pattern)) {
            positions.push_back(i);
        }
    }
    return positions;
}

int main() {
    std::string pattern, text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);
    std::vector<int> positions = findMatches(pattern, text);
    for (size_t i = 0; i < positions.size(); ++i) {
        std::cout << positions[i];
        if (i != positions.size() - 1) {
            std::cout << " ";
        }
    }

    return 0;
}
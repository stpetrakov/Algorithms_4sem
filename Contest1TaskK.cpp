// pattern_matching.cpp
// Linear-time wildcard pattern matching using an Extended Z-algorithm
// '?' inside the pattern matches any single character in the text.

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

//--------------------------------------------------------------
//  Utility
//--------------------------------------------------------------
/// @brief Return true iff patternChar matches textChar under the wildcard rules.
inline bool isWildcardMatch(char patternChar, char textChar) {
    return patternChar == '?' || patternChar == textChar;
}

//--------------------------------------------------------------
//  Z-box on the pattern itself (classic Z / "next" array)
//--------------------------------------------------------------
/// @brief Build the classical Z-array for the pattern, but with wildcard support.
///        z[i] = longest length such that pattern[0 .. z[i) == pattern[i .. i+z[i))
std::vector<int> buildZArray(const std::string &pattern) {
    const int patternLength = static_cast<int>(pattern.size());
    std::vector<int> z(patternLength);
    z[0] = patternLength;                       // by definition

    int left = 0, right = 0;                    // current [left, right] Z-box
    for (int idx = 1; idx < patternLength; ++idx) {
        int currentMatchLen = 0;

        // 1. possible reuse of previous Z-box information
        if (idx <= right) {
            const int mirrored = idx - left;    // position inside current Z-box
            currentMatchLen = std::min(z[mirrored], right - idx + 1);
        }

        // 2. explicit comparisons beyond the currentMatchLen prefix
        while (idx + currentMatchLen < patternLength &&
               isWildcardMatch(pattern[currentMatchLen], pattern[idx + currentMatchLen])) {
            ++currentMatchLen;
        }
        z[idx] = currentMatchLen;

        // 3. extend the Z-box if we improved it
        if (idx + currentMatchLen - 1 > right) {
            left  = idx;
            right = idx + currentMatchLen - 1;
        }
    }
    return z;
}

//--------------------------------------------------------------
//  Extended Z (pattern vs. text)   —   the core of ex-KMP
//--------------------------------------------------------------
/// @brief Compute ext[i] = longest prefix of pattern matching text starting at i.
std::vector<int> buildExtendedArray(const std::string &pattern,
                                    const std::string &text,
                                    const std::vector<int> &zPattern) {
    const int patternLength = static_cast<int>(pattern.size());
    const int textLength    = static_cast<int>(text.size());

    std::vector<int> ext(textLength);
    int left = 0, right = -1;                   // current [left, right] match window in text

    for (int idx = 0; idx < textLength; ++idx) {
        int currentMatchLen = 0;

        // 1. potentially reuse knowledge from previous window
        if (idx <= right) {
            const int mirrored = idx - left;
            currentMatchLen = std::min(zPattern[mirrored], right - idx + 1);
        }

        // 2. explicit comparisons beyond what we already know
        while (currentMatchLen < patternLength &&
               idx + currentMatchLen < textLength &&
               isWildcardMatch(pattern[currentMatchLen], text[idx + currentMatchLen])) {
            ++currentMatchLen;
        }
        ext[idx] = currentMatchLen;

        // 3. extend the current window if we improved it
        if (idx + currentMatchLen - 1 > right) {
            left  = idx;
            right = idx + currentMatchLen - 1;
        }
    }
    return ext;
}

//--------------------------------------------------------------
//  Top-level matching routine
//--------------------------------------------------------------
/// @brief Return all starting positions where pattern matches text under '?'.
std::vector<int> findMatches(const std::string &pattern, const std::string &text) {
    const int patternLength = static_cast<int>(pattern.size());
    const int textLength    = static_cast<int>(text.size());
    std::vector<int> matches;

    // Trivial case: pattern is entirely wildcards – matches everywhere it fits.
    if (std::count(pattern.begin(), pattern.end(), '?') == patternLength) {
        matches.reserve(textLength - patternLength + 1);
        for (int pos = 0; pos + patternLength <= textLength; ++pos) {
            matches.push_back(pos);
        }
        return matches;
    }

    // Forward prefix matches
    const auto zForward   = buildZArray(pattern);
    const auto extForward = buildExtendedArray(pattern, text, zForward);

    // Suffix matches by running the same on reversed strings
    std::string patternReversed(pattern.rbegin(), pattern.rend());
    std::string textReversed(text.rbegin(), text.rend());

    const auto zReverse   = buildZArray(patternReversed);
    const auto extReverse = buildExtendedArray(patternReversed, textReversed, zReverse);

    // extReverse is aligned to reversed text; convert it so that extSuffix[i] = suffix length at position i.
    std::vector<int> extSuffix(textLength);
    for (int idx = 0; idx < textLength; ++idx) {
        extSuffix[textLength - 1 - idx] = extReverse[idx];
    }

    // A position matches if its forward-prefix + backward-suffix cover the entire pattern.
    for (int pos = 0; pos + patternLength <= textLength; ++pos) {
        const int prefixLen  = extForward[pos];
        const int suffixLen  = extSuffix[pos + patternLength - 1];
        if (prefixLen + suffixLen >= patternLength) {
            matches.push_back(pos);
        }
    }
    return matches;
}

//--------------------------------------------------------------
//  I/O helpers
//--------------------------------------------------------------
void printPositions(const std::vector<int> &positions) {
    for (std::size_t idx = 0; idx < positions.size(); ++idx) {
        if (idx) std::cout << ' ';
        std::cout << positions[idx];
    }
    std::cout << '\n';
}

//--------------------------------------------------------------
//  Driver
//--------------------------------------------------------------
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string pattern;
    std::string text;
    std::getline(std::cin, pattern);
    std::getline(std::cin, text);

    const auto positions = findMatches(pattern, text);
    printPositions(positions);

    return 0;
}

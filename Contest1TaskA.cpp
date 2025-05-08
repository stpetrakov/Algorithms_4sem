#include <iostream>
#include <vector>
#include <iterator>
#include <type_traits>

template <typename It>
std::vector<std::size_t> computePrefixFunction(It begin, It end) {
    using Category = typename std::iterator_traits<It>::iterator_category;
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, Category>,
                  "Iterator must satisfy RandomAccessIterator");

    const std::size_t length = static_cast<std::size_t>(end - begin);
    std::vector<std::size_t> prefix(length);

    for (std::size_t i = 1; i < length; ++i) {
        std::size_t border = prefix[i - 1];
        while (border > 0 && *(begin + i) != *(begin + border))
            border = prefix[border - 1];
        if (*(begin + i) == *(begin + border))
            ++border;
        prefix[i] = border;
    }
    return prefix;
}

template <typename PatIt, typename TextIt>
std::vector<std::size_t> kmpSearchOccurrences(PatIt patBegin, PatIt patEnd,
                                              TextIt txtBegin, TextIt txtEnd) {
    using PatCat  = typename std::iterator_traits<PatIt>::iterator_category;
    using TxtCat  = typename std::iterator_traits<TextIt>::iterator_category;
    static_assert(std::is_base_of_v<std::random_access_iterator_tag, PatCat> &&
                  std::is_base_of_v<std::random_access_iterator_tag, TxtCat>,
                  "Iterators must satisfy RandomAccessIterator");

    const std::size_t patternSize = static_cast<std::size_t>(patEnd - patBegin);
    if (patternSize == 0) return {};

    const auto prefix = computePrefixFunction(patBegin, patEnd);
    std::vector<std::size_t> occurrences;

    std::size_t border = 0;
    const std::size_t textSize = static_cast<std::size_t>(txtEnd - txtBegin);
    for (std::size_t i = 0; i < textSize; ++i) {
        while (border > 0 && *(txtBegin + i) != *(patBegin + border))
            border = prefix[border - 1];
        if (*(txtBegin + i) == *(patBegin + border))
            ++border;
        if (border == patternSize) {
            occurrences.push_back(i - patternSize + 1);
            border = prefix[border - 1];
        }
    }
    return occurrences;
}

template <typename T>
std::ostream& printPositions(std::ostream& os,
                             const std::vector<T>& positions,
                             char sep = '\n') {
    for (const auto& pos : positions) os << pos << sep;
    return os;
}

int main() {
    std::string text;
    std::string pattern;
    std::cin >> text >> pattern;

    const auto positions = kmpSearchOccurrences(pattern.begin(), pattern.end(),
                                                text.begin(),    text.end());

    printPositions(std::cout, positions);
    return 0;
}

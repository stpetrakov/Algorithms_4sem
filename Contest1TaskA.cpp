#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<size_t> prefix_function (std::string str) 
{
    size_t len =  str.length();
    std::vector<size_t> pi(len);

    for (size_t i = 1; i < len; ++i) 
    {
        size_t j = pi[i-1];
        while ((j > 0) && (str[i] != str[j]))
            j = pi[j - 1];

        if (str[i] == str[j]) 
            ++j;

        pi[i] = j;
     }
     return pi;
} 

int main() {
    std::string str_a;
    std::string str_b;

    std::cin >> str_a;
    std::cin >> str_b;

    std::string str = str_b + "@" + str_a;
    std::vector<size_t> pref;

    pref = prefix_function(str);
    size_t len_b = str_b.length();

    for (size_t i = 0; i < pref.size(); ++i) {
        if (pref[i] == len_b)
            std::cout << i - len_b * 2 << '\n';
    }

    return 0;
}
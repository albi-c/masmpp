#ifndef MASMPP_STRUTIL_HPP
#define MASMPP_STRUTIL_HPP

#include <string>
#include <vector>

template<typename Char, typename Traits, typename Allocator>
std::basic_string<Char, Traits, Allocator> operator *
(const std::basic_string<Char, Traits, Allocator> s, size_t n) {
   std::basic_string<Char, Traits, Allocator> tmp = s;
   for (size_t i = 0; i < n; ++i)
   {
      tmp += s;
   }
   return tmp;
}

template<typename Char, typename Traits, typename Allocator>
std::basic_string<Char, Traits, Allocator> operator *
(size_t n, const std::basic_string<Char, Traits, Allocator>& s) {
   return s * n;
}

namespace su {
    /**
     * Splits a string by delim
     * Doesn't split between quotes
     * 
     * @param str string to split
     * @param delim delimiter to split by
     */
    std::vector<std::string> split(const std::string &str, char delim);

    /**
     * Replaces a substring by another string
     * 
     * @param str string to replace in
     * @param from string being replaced
     * @param to string to change to
     */
    std::string replace(std::string &str, std::string from, std::string to);
};

#endif

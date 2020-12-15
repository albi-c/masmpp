#ifndef MASMPP_STRUTIL_HPP
#define MASMPP_STRUTIL_HPP

#include <string>
#include <vector>

namespace su {
    /**
     * Splits a string by delim
     * Doesn't split between quotes
     * 
     * @param str string to split
     * @param delim delimiter to split by
     */
    std::vector<std::string> split(std::string &str, char delim);

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

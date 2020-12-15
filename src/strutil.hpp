#ifndef MASMPP_STRUTIL_HPP
#define MASMPP_STRUTIL_HPP

#include <string>
#include <vector>

namespace su {
    std::vector<std::string> split(std::string &s, char delim);
    std::string replace(std::string &str, std::string from, std::string to);
};

#endif

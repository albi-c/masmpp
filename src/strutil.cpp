#include "strutil.hpp"

std::vector<std::string> su::split(const std::string &str, char delim, int limit) {
    std::vector<std::string> v;
    
    std::string tok;
    bool quotes = false;
    int splcount = 0;
    for (char c : str) {
        if (c == ' ' && !quotes) {
            if (splcount >= limit && limit >= 0)
                continue;
            v.push_back(tok);
            tok = "";
            splcount++;
        } else if (c == '"') {
            quotes = !quotes;
            tok += c;
        } else {
            tok += c;
        }
    }
    if (!tok.empty())
        v.push_back(tok);

    return v;
}

std::string su::replace(std::string str, std::string from, std::string to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str = str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

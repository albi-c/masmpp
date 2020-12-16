#ifndef MASMPP_STDIOSTREAM_HPP
#define MASMPP_STDIOSTREAM_HPP

#include <string>
#include <iostream>

#include "iostream.hpp"

class STDIOStream : public IOStream {
public:
    std::string read() {
        std::string text;
        for (std::string line; std::getline(std::cin, line); ) {
            text += line + "\n";
        }
        return text;
    }

    void write(std::string text) {
        std::cout << text;
    }
};

#endif

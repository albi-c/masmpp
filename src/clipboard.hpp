#ifndef MASMPP_CLIPBOARD_HPP
#define MASMPP_CLIPBOARD_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
 
#include "logger.hpp"
#include "iostream.hpp"

#if defined(linux) || defined(__linux__) || defined(__linux__)
#include <stdio.h>
#elif defined(_WIN32) || defined(_WIN64)
// TODO
#endif

std::string clipboardRead();
void clipboardWrite(std::string text);

class Clipboard : public IOStream {
public:
    std::string read() {
        return clipboardRead();
    }

    void write(std::string text) {
        clipboardWrite(text);
    }
};

#endif

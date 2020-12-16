#ifndef MASMPP_IOSTREAM_HPP
#define MASMPP_IOSTREAM_HPP

#include <string>

class IOStream {
public:
    virtual std::string read() { return ""; };
    virtual void write(std::string text) {};
};

#endif

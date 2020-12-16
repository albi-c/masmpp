#include <string>
#include <iostream>

#include "libmasm++.hpp"
#include "logger.hpp"

const std::string text2 = "print [1 + 2 - 3] [ 4 + 5 - 6 ]\n"
    "print  \"[ 1 + 2 - 3 ]\"\n"
    "print [ 1 + 2 - 3 ]\n"
    "printflush message1";

void IOwrite(std::ostream &stream, std::string text) {
    stream << text;
}
std::string IOread(std::istream &stream) {
    std::string text;
    for (std::string line; std::getline(std::cin, line); ) {
        text += line + "\n";
    }
    return text;
}

int main(int argc, char* argv[]) {
    std::istream* input = &std::cin;
    std::ostream* output = &std::cout;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        
        if (arg == "-i:s")
            input = &std::cin;
        if (arg == "-o:s")
            output = &std::cout;
    }

    // std::string text = IOread(*input);

    masmpp::Preprocessor proc(text2);

    if (proc.process() != 0) {
        log::e(proc.getError());
    }

    IOwrite(*output, proc.getText());

    return 0;
}

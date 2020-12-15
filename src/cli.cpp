#include "libmasm++.hpp"

#include <string>
#include <iostream>

using namespace masmpp;

const std::string text2 = ".func myprint text text2\n"
    "print $text\n"
    "print $text2\n"
    ".ret 123\n"
    ".call myprint \"frog1 \" \"frog2\"\n"
    "print $RET\n"
    "printflush message1\n";

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

    std::string text = IOread(*input);

    Preprocessor proc(text);

    if (proc.process() != 0) {
        std::cout << proc.getError() << "\n";
    }

    IOwrite(*output, proc.getText());

    return 0;
}

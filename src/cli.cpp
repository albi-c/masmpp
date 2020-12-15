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

class IODevice {
public:
    void write(std::string text) {}
    std::string read() { return ""; }
};

class STDIO : public IODevice {
public:
    void write(std::string text) {
        std::cout << text;
    }
    std::string read() {
        std::string text;
        for (std::string line; std::getline(std::cin, line); ) {
            text += line + "\n";
        }
        return text;
    }
};

int main(int argc, char* argv[]) {
    IODevice input = STDIO();
    IODevice output = STDIO();

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        
        if (arg == "-i:s")
            input = STDIO();
        if (arg == "-o:s")
            output = STDIO();
    }

    std::string text = input.read();

    Preprocessor proc(text);
    proc.setOptions(PreprocessOptions::INLINE_OPERATIONS | PreprocessOptions::FUNCTIONS | PreprocessOptions::LABELS);

    if (proc.process() != 0) {
        std::cout << proc.getError() << "\n";
    }

    output.write(proc.getText());

    return 0;
}

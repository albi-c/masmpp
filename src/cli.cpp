#include "libmasm++.hpp"

#include <string>
#include <iostream>

using namespace masmpp;

const std::string text = ".label a\n"
    "\n"
    "print \"h1\"\n\n"
    ".label b_1_c\n"
    "\nprint \"h2\"\n"
    ".jump b_1_c\n"
    "printflush message1\n"
    ".label b_1_c";

const std::string text2 = ".func myprint text text2\n"
    "print $text\n"
    "print $text2\n"
    ".ret\n"
    ".call myprint \"frog1 \" \"frog2\"\n"
    "printflush message1\n";

int main(int argc, char* argv[]) {
    Preprocessor proc(text2);
    proc.setOptions(PreprocessOptions::INLINE_OPERATIONS | PreprocessOptions::FUNCTIONS | PreprocessOptions::LABELS);

    if (proc.process() != 0) {
        std::cout << proc.getError() << "\n";
    }

    std::cout << proc.getText();

    return 0;
}

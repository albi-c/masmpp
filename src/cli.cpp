#include <string>
#include <iostream>

#include "libmasm++.hpp"
#include "logger.hpp"
#include "iostream.hpp"
#include "clipboard.hpp"
#include "stdiostream.hpp"
#include "strutil.hpp"

const std::string helpMessage = "Usage:\n"
    "  %cmd [--help | -h] [-i:s] [-o:s] [-i:c] [-o:c]\n"
    "\nParameters:\n"
    "  --help -h Display this message and exit\n\n"
    "  -i:s Get input from STDIN\n"
    "  -o:s Print output to STDOUT\n"
    "  -i:c Get input from clipboard\n"
    "  -o:c Write input to clipboard\n";

const std::string text2 = "print [1 + 2 - 3] [ 4 + 5 - 6 ]\n"
    "print  \"[ 1 + 2 - 3 ]\"\n"
    "print [ 1 + 2 - 3 ]\n"
    "printflush message1";

int main(int argc, char* argv[]) {
    Clipboard clipboard;
    STDIOStream stdiostream;
    IOStream* input = &stdiostream;
    IOStream* output = &stdiostream;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        
        if (arg == "-i:s")
            input = &stdiostream;
        if (arg == "-o:s")
            output = &stdiostream;
        
        if (arg == "-i:c")
            input = &clipboard;
        if (arg == "-o:c")
            output = &clipboard;
        
        if (arg == "--help" | arg == "-h") {
            std::cout << su::replace(helpMessage, "%cmd", argv[0]);
            return 0;
        }
    }

    std::string text = input->read();

    masmpp::Preprocessor proc(text);

    if (proc.process() != 0) {
        log::e(proc.getError());
    }

    output->write(proc.getText());

    return 0;
}

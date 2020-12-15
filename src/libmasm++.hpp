#ifndef MASMPP_LIBMASMPP_HPP
#define MASMPP_LIBMASMPP_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <regex>
#include <vector>
#include <cstdlib>
#include <iomanip>

namespace masmpp {
    enum PreprocessOptions {
        INLINE_OPERATIONS = 0b1,
        LABELS = 0b10,
        FUNCTIONS = 0b100,
        IF = 0b1000
    };

    struct Function {
        std::string name;
        std::vector<std::string> params;
    };

    struct InlineOperation {};

    class Preprocessor {
    private:
        const std::regex R_LABEL;
        const std::regex R_JUMP;
        const std::regex R_CJUMP;
        const std::regex R_RCJUMP;
        const std::regex R_FUNC;
        const std::regex R_CALL;
        const std::regex R_RET;
        const std::regex R_FBEGIN;
        const std::regex R_FVAR;
        const std::regex R_IF;
        const std::regex R_ELSE;
        const std::regex R_EIF;

        const std::map<std::string, std::string> constants = {
            {"$RET", "MASMPP_FUNC_RET"}
        };

        std::string text;

        int options;

        std::string last_error;

        std::vector<std::string> split(std::string &s, char delim);
        std::string replace(std::string &str, std::string from, std::string to);
        InlineOperation* findInlineOp(std::string str, size_t begin = 0);
        int genID();
    public:
        Preprocessor(std::string text = "");

        void setText(std::string text);
        std::string getText();

        void setOptions(int options);
        int getOptions();

        int process();

        std::string getError();
    };
}

#endif

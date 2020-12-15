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

#include "strutil.hpp"

namespace masmpp {
    /**
     * Preprocessor options - enables or disables various functions
     */
    enum PreprocessOptions {
        INLINE_OPERATIONS = 0b1,
        LABELS = 0b10,
        FUNCTIONS = 0b100,
        IF = 0b1000
    };

    /**
     * Function defined in code with .func <name> [param...]
     */
    struct Function {
        std::string name;
        std::vector<std::string> params;
    };

    /**
     * Element of an inline operation
     */
    struct OperationElement {
        std::string type;
        std::string val;
    };

    /**
     * Inline operation, stores its elements
     */
    struct InlineOperation {
        std::vector<OperationElement> elements;

        std::string text;

        size_t startPos;
        size_t endPos;

        bool error = false;
        bool empty = true;
    };

    /**
     * Main Preprocessor class
     */
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

        const std::vector<PreprocessOptions> disabledOperations = {
            PreprocessOptions::INLINE_OPERATIONS
        };

        std::string text;

        int options;

        std::string last_error;

        /**
         * Parses inline operation
         * @param str string to parse
         *            example: [ a + b - c * d ]
         */
        std::vector<OperationElement> parseInlineOp(std::string &str);

        /**
         * Find inline operation
         * @param str string to find inline operation in
         * @param begin where to start searching from
         */
        InlineOperation findInlineOp(std::string &str, size_t begin = 0);
        int genID();
    public:
        Preprocessor(std::string text = "", int options = 0xffff);

        /**
         * Set text to process
         * @param text
         */
        void setText(std::string text);

        /**
         * Get text (processed if process() called after setting text)
         */
        std::string getText();

        /**
         * Set preprocessor options
         */
        void setOptions(int options);

        /**
         * Get preprocessor options
         */
        int getOptions();

        /**
         * Process the text
         * Returns 1 for error and 0 for OK
         */
        int process();

        /**
         * Returns last error
         */
        std::string getError();
    };
}

#endif

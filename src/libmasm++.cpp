#include "libmasm++.hpp"

masmpp::Preprocessor::Preprocessor(std::string text, int options)
    : R_LABEL("^\\.label \\w+$"), R_JUMP("^\\.jump \\w+$"), R_CJUMP("^\\.jump \\w+ \\w+$"), R_RCJUMP("^\\.jump \\w+ \\!\\w+$"),
    R_FUNC("^\\.func \\w+((( \\w+)+))?$"), R_CALL("^\\.call \\w+((( [\\w\"\\\\]+)+))?$"), R_RET("^\\.ret( (\\w+))?$"),
    R_FBEGIN("^\\.funcbegin \\w+$"), R_FVAR("\\$\\w+(( \\$\\w+)+)?"), R_IF("^.if \\w+$"), R_ELSE("^.else$"), R_EIF("^.eif$"),
    R_DEF("^\\.DEF \\w+ .+$"), R_REP("^\\.REP \\d+$"), R_EREP("^\\.EREP$"),
    text(text), options(options) {}

std::string masmpp::Preprocessor::genInlineOp(InlineOperation &operation) {
    std::vector<OperationElement> elements = operation.elements;
    std::string varName = operation.varName;

    std::string out;
    OperationElement lastElem;

    for (auto &elem : elements) {
        if (elem.val.empty())
            continue;
        if (elem.type == OperationElementType::OP) {
            if (out.empty()) {
                last_error = "Inline operation cannot start with an operator";
                return "ERROR";
            }
            if (lastElem.type == OperationElementType::OP) {
                last_error = "Operator must be followed by value, not another operator";
                return "ERROR";
            }
        } else if (elem.type == OperationElementType::VAL) {
            if (lastElem.type == OperationElementType::VAL) {
                last_error = "Value must be followed by operator, not another value";
                return "ERROR";
            }

            if (out.empty()) {
                out += "set " + varName + " " + elem.val + "\n";
            } else {
                out += "op " + inlineOperations[lastElem.val] + " " + varName + " " + varName + " " + elem.val + "\n";
            }
        }

        lastElem = elem;
    }

    return out;
}

std::vector<masmpp::OperationElement> masmpp::Preprocessor::parseInlineOp(const std::string &str) {
    std::vector<std::string> spl = su::split(str, ' ');

    std::vector<masmpp::OperationElement> result;

    for (auto &s : spl) {
        OperationElement el;
        el.val = s;
        if (inlineOperations.find(s) != inlineOperations.end()) {
            el.type = OperationElementType::OP;
        } else {
            el.type = OperationElementType::VAL;
        }
        result.push_back(el);
    }

    return result;
}

masmpp::InlineOperation masmpp::Preprocessor::findInlineOp(std::string &str, size_t begin) {
    // start and end positions of inline operation
    size_t startpos;
    size_t endpos;

    // was inline operation found yet
    bool found = false;

    bool instring = false;

    size_t i = 0;
    for (char &c : str) {
        if (i == 0 && c == '#')
            break;
        if (i < begin) {
            i++;
            continue;
        }
        if (c == '[' && !instring) {
            // nested inline operations not yet supported, error
            if (found) {
                last_error = "Nested inline operations not yet supported";
                InlineOperation ret;
                ret.error = true;
                return ret;
            }

            found = true;
            startpos = i;
        } else if (c == ']' && !instring) {
            // cannot close inline operation without opening it first
            if (!found) {
                last_error = "Found closing bracket without a corresponding opening one";
                InlineOperation ret;
                ret.error = true;
                return ret;
            }

            endpos = i;

            // return an inline operation
            InlineOperation ret;
            ret.text = str.substr(startpos, endpos - startpos + 1);
            ret.startPos = startpos;
            ret.endPos = endpos;
            ret.varName = "MASMPP_IOP_" + std::to_string(genID());
            ret.elements = parseInlineOp(ret.text.substr(1, ret.text.length() - 2));
            ret.empty = false;

            return ret;
        } else if (c == '"') {
            instring = !instring;
        }

        i++;
    }

    return InlineOperation();
}

int masmpp::Preprocessor::genID() {
    // return a random number between 0 and 999999
    return (rand() % 999999) - 1;
}

void masmpp::Preprocessor::setText(std::string text) {
    this->text = text;
}
std::string masmpp::Preprocessor::getText() {
    return text;
}

void masmpp::Preprocessor::setOptions(int options) {
    this->options = options;
}
int masmpp::Preprocessor::getOptions() {
    return options;
}

int masmpp::Preprocessor::process() {
    // disable certain operations
    for (int op : disabledOperations) {
        options &= ~op;
    }

    if ((options & PreprocessOptions::FUNCTIONS || options & PreprocessOptions::IF) && !(options & PreprocessOptions::LABELS)) {
        log::w("LABELS are required for FUNCTIONS and IF, enabling");
        options |= PreprocessOptions::LABELS;
    }

    if (options & PreprocessOptions::REPEAT) {
        std::string out;

        std::string chunk;
        bool inrep;
        int repnum;

        std::istringstream iss(text);
        for (std::string line; std::getline(iss, line); ) {
            if (regex_match(line, R_REP)) {
                if (inrep) {
                    last_error = "Nested repeats not yet supported";
                    return 1;
                }

                std::vector<std::string> spl = su::split(line, ' ');
                repnum = std::stoi(spl[1]);

                inrep = true;
            } else if (regex_match(line, R_EREP)) {
                if (!inrep) {
                    last_error = "Closing repeat without opening it";
                    return 1;
                }

                if (repnum > 0)
                    out += chunk * (repnum - 1);

                chunk = "";
                inrep = false;
            } else {
                if (inrep) {
                    chunk += line + "\n";
                } else {
                    out += line + "\n";
                }
            }
        }

        text = out;
    }

    if (options & PreprocessOptions::CONST) {
        std::string out;

        std::istringstream iss(text);
        for (std::string line; std::getline(iss, line); ) {
            if (regex_match(line, R_DEF)) {
                std::vector<std::string> spl = su::split(line, ' ', 2);

                if (constants.find(spl[1]) != constants.end()) {
                    last_error = "Cannot overwrite a constant";
                    return 1;
                }

                constants[spl[1]] = spl[2];
            } else {
                out += line + "\n";
            }
        }

        text = out;
    }

    if (options & PreprocessOptions::INLINE_OPERATIONS) {
        std::string out;

        std::istringstream iss(text);
        for (std::string line; std::getline(iss, line); ) {
            InlineOperation op;
            std::vector<InlineOperation> ops;
            size_t lastPos = 0;
            while (true) {
                op = findInlineOp(line, lastPos);
                if (op.error)
                    return 1;
                if (op.empty)
                    break;
                
                lastPos = op.endPos + 1;

                ops.push_back(op);
                
                std::string txt = genInlineOp(op);
                out += txt + "\n";
            }

            for (auto op : ops) {
                line = su::replace(line, op.text, op.varName);
            }
            out += line + "\n";
        }

        text = out + "\n";
    }
    
    if (options & PreprocessOptions::FUNCTIONS) {
        std::map<std::string, Function> functions;

        std::string out;

        int i = 0;
        std::string fname;

        std::istringstream iss(text);
        for (std::string line; std::getline(iss, line); ) {
            if (line.empty())
                continue;

            if (std::regex_match(line, R_FUNC)) {
                std::vector<std::string> toks = su::split(line, ' ');
                fname = toks[1];
                functions[fname].name = fname;
                functions[fname].params = std::vector<std::string>(toks.begin() + 2, toks.end());

                out += ".funcbegin " + fname + "\n";
                out += ".label MASMPP_FUNC_" + fname + "\n";
            } else {
                out += line + "\n";
            }

            i++;
        }

        text = out;
        out = "";

        iss = std::istringstream(text);
        for (std::string line; std::getline(iss, line); ) {
            if (std::regex_match(line, R_CALL)) {
                std::vector<std::string> toks = su::split(line, ' ');
                fname = toks[1];
                functions[fname].name = fname;
                std::vector<std::string> params = std::vector<std::string>(toks.begin() + 2, toks.end());
                
                int i = 0;
                for (std::string param : functions[fname].params) {
                    if (params.size() != functions[fname].params.size()) {
                        last_error = "Incorrect number of parameters in call to " + fname;
                        return 1;
                    }
                    out += "set MASMPP_FUNC_" + fname + "_PARAM_" + param + " " + params[i] + "\n";
                    i++;
                }
                out += "op add MASMPP_FUNC_" + fname + "_raddr " + "@counter 1" + "\n";
                out += ".jump MASMPP_FUNC_" + fname + "\n";
            } else {
                out += line + "\n";
            }

            i++;
        }

        text = out;
        out = "";

        std::vector<std::string> fnames;
        size_t pos;

        iss = std::istringstream(text);
        for (std::string line; std::getline(iss, line); ) {
            std::smatch fname_match;
            if (std::regex_search(line, fname_match, R_FVAR) && fnames.size() > 0) {
                std::string fname = fnames.back();

                for (std::string param : functions[fname].params) {
                    while ((pos = line.find("$" + param)) != std::string::npos) {
                        line = line.substr(0, pos) + "MASMPP_FUNC_" + fname + "_PARAM_" + param + line.substr(pos + param.size() + 1);
                    }
                }
            }

            if (std::regex_match(line, R_RET)) {
                if (fnames.size() <= 0) {
                    last_error = "Returning from main scope";
                    return 1;
                } else {
                    std::vector<std::string> spl;
                    spl = su::split(line, ' ');
                    if (spl.size() > 1)
                        out += "set MASMPP_FUNC_RET " + spl[1] + "\n";

                    out += "set @counter MASMPP_FUNC_" + fnames.back() + "_raddr\n";
                    out += ".label MASMPP_FUNC_" + fnames.back() + "_ret\n";
                    fnames.pop_back();
                }
            } else if (std::regex_match(line, R_FBEGIN)) {
                line.erase(0, line.find(" ") + 1);

                fnames.push_back(line);

                out += ".jump MASMPP_FUNC_" + line + "_ret\n";
            } else {
                out += line + "\n";
            }

            i++;
        }

        for (auto const& [key, val] : constants) {
            out = su::replace(out, key, val);
        }

        text = out;
    }

    if (options & PreprocessOptions::IF) {
        std::vector<int> ifs;
        std::map<int, bool> hasElse;

        std::string out;

        std::istringstream iss(text);
        for (std::string line; std::getline(iss, line); ) {
            if (std::regex_match(line, R_IF)) {
                line.erase(0, line.find(" ") + 1);

                int id = genID();

                ifs.push_back(id);
                hasElse[id] = false;

                out += ".jump MASMPP_IF_" + std::to_string(id) + "_else !" + line + "\n";
            } else if (std::regex_match(line, R_ELSE)) {
                if (ifs.size() <= 0) {
                    last_error = "Else without opening if";
                    return 1;
                }

                int id = ifs.back();

                hasElse[id] = true;

                out += ".jump MASMPP_IF_" + std::to_string(id) + "_end\n";
                out += ".label MASMPP_IF_" + std::to_string(id) + "_else\n";
            } else if (std::regex_match(line, R_EIF)) {
                if (ifs.size() <= 0) {
                    last_error = "Eif without opening if";
                    return 1;
                }

                int id = ifs.back();
                ifs.pop_back();

                if (!hasElse[id])
                    out += ".label MASMPP_IF_" + std::to_string(id) + "_else\n";
                out += ".label MASMPP_IF_" + std::to_string(id) + "_end\n";
            } else {
                out += line + "\n";
            }
        }

        text = out;
    }

    if (options & PreprocessOptions::LABELS) {
        std::map<std::string, int> labels;

        std::string out;

        int i = 0;

        std::istringstream iss(text);
        for (std::string line; std::getline(iss, line); ) {
            if (line.empty())
                continue;

            if (std::regex_match(line, R_LABEL)) {
                line.erase(0, line.find(" ") + 1);

                labels[line] = i;
                out += "jump " + std::to_string(i + 1) + " always 0 0\n";
            } else {
                out += line + "\n";
            }

            i++;
        }

        text = out;
        out = "";

        iss = std::istringstream(text);
        for (std::string line; std::getline(iss, line); ) {
            if (std::regex_match(line, R_JUMP)) {
                line.erase(0, line.find(" ") + 1);

                if (!labels.count(line)) {
                    last_error = "Label \"" + line + "\" not found!";
                    return 1;
                }

                out += "jump " + std::to_string(labels[line]) + " always 0 0\n";
            } else if (std::regex_match(line, R_RCJUMP)) {
                std::vector<std::string> spl = su::split(line, ' ');

                if (!labels.count(spl[1])) {
                    last_error = "Label \"" + line + "\" not found!";
                    return 1;
                }

                out += "jump " + std::to_string(labels[spl[1]]) + " equal " + spl[2].substr(1) + " false\n";
            } else if (std::regex_match(line, R_CJUMP)) {
                std::vector<std::string> spl = su::split(line, ' ');

                if (!labels.count(spl[1])) {
                    last_error = "Label \"" + line + "\" not found!";
                    return 1;
                }

                out += "jump " + std::to_string(labels[spl[1]]) + " equal " + spl[2] + " true\n";
            } else {
                out += line + "\n";
            }

            i++;
        }

        text = out;
    }
    return 0;
}

std::string masmpp::Preprocessor::getError() {
    return last_error;
}

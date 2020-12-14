#include "libmasm++.hpp"

masmpp::Preprocessor::Preprocessor(std::string text)
    : R_LABEL("^\\.label \\w+$"), R_JUMP("^\\.jump \\w+$"), R_FUNC("^\\.func \\w+((( \\w+)+))?$"),
    R_CALL("^\\.call \\w+((( [\\w\"\\\\]+)+))?$"), R_RET("^\\.ret( (\\w+))?$"), R_FBEGIN("^\\.funcbegin \\w+$"),
    R_FVAR("\\$\\w+(( \\$\\w+)+)?"),
    text(text) {}

std::vector<std::string> masmpp::Preprocessor::split(std::string &str, char delim) {
    std::vector<std::string> v;
    
    std::string tok;
    bool quotes = false;
    for (char &c : str) {
        if (c == ' ' && !quotes) {
            v.push_back(tok);
            tok = "";
        } else if (c == '"') {
            quotes = !quotes;
            tok += c;
        } else {
            tok += c;
        }
    }
    if (!tok.empty())
        v.push_back(tok);

    return v;
}

std::string masmpp::Preprocessor::replace(std::string str, std::string from, std::string to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
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
    if (options & PreprocessOptions::INLINE_OPERATIONS) {
        // TODO
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
                std::vector<std::string> toks = split(line, ' ');
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
                std::vector<std::string> toks = split(line, ' ');
                fname = toks[1];
                functions[fname].name = fname;
                std::vector<std::string> params = std::vector<std::string>(toks.begin() + 2, toks.end());
                
                int i = 0;
                for (std::string param : functions[fname].params) {
                    if (params.size() < functions[fname].params.size()) {
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
                    spl = split(line, ' ');
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
            out = replace(out, key, val);
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

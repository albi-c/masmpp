#include "logger.hpp"

std::string log::getTime() {
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), LogInfo().logTimeFormat, timeinfo);
    return std::string(buffer);
}

void log::log(std::string msg, LogLevel level) {
    std::string out = LogInfo().logFormat;

    out = su::replace(out, "%t", getTime());
    out = su::replace(out, "%c", LogInfo().colors[LogInfo().logLevelLetter[level]]);
    out = su::replace(out, "%i", std::string(&LogInfo().logLevelLetter[level]));
    out = su::replace(out, "%m", msg);
    out = su::replace(out, "%e", LogInfo().colors['R']);

    std::cout << out;
}

void log::d(std::string msg) {
    log(msg, LogLevel::DEBUG);
}
void log::i(std::string msg) {
    log(msg, LogLevel::INFO);
}
void log::w(std::string msg) {
    log(msg, LogLevel::WARN);
}
void log::e(std::string msg) {
    log(msg, LogLevel::ERROR);
}

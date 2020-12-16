#ifndef MASMPP_LOGGER_HPP
#define MASMPP_LOGGER_HPP

#include <iostream>
#include <map>
#include <ctime>

#include "strutil.hpp"

namespace log {
    /**
     * Logging levels
     */
    enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    struct LogInfo {
        /**
         * Log message format
         * Variables: %t - time, %m - message, %i - importance, %c - start color, %e - end color
         */
        const std::string logFormat = "[%t] %c[%i]%e %m\n";

        /**
         * Format of time in messages
         */
        const char* logTimeFormat = "%d/%m/%Y %H:%M:%S";

        /**
         * Colors used in log messages
         */
        std::map<char, std::string> colors = {
            {'R', "\033[0m"},

            {'D', "\033[0;34m"},
            {'I', "\033[0;32m"},
            {'W', "\033[0;33m"},
            {'E', "\033[1;31m"}
        };

        /**
         * Letters representing log levels
         */
        std::map<LogLevel, char> logLevelLetter = {
            {LogLevel::DEBUG, 'D'},
            {LogLevel::INFO , 'I'},
            {LogLevel::WARN , 'W'},
            {LogLevel::ERROR, 'E'}
        };
    };

    /**
     * Return time as a formatted string
     */
    std::string getTime();

    /**
     * Log a message to stdout
     */
    void log(std::string msg, LogLevel level);

    void d(std::string msg);
    void i(std::string msg);
    void w(std::string msg);
    void e(std::string msg);
};

#endif

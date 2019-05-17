#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <string>
#include <iostream>

namespace logging {
    void log(std::string message) {
        time_t t = time(0);
        struct tm* tmp = localtime(&t);
        char time_string[50];
        strftime(time_string, 50, "[%x - %I:%M:%S] ", tmp);
        std::cout << time_string << message << std::endl;
    }
}

#endif

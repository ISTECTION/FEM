#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_
#include <source_location>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <ctime>
#include <tuple>

#define _LOG_BEGIN namespace Log {
#define _LOG_END                 }

_LOG_BEGIN

using ::std::source_location;

class Logger
{
private:
    static std::string fileName;
    static std::string getNameLog();
public:
    static bool append (const std::string &line);
};

std::string Logger::fileName = Logger::getNameLog();

bool checkFile (const std::ifstream &fin, const std::string &error) {

    if (fin.is_open()) return true;
    else {
        Logger::append(error);
        return false;
    }
}

std::string getLog(const std::string &message,
                   const source_location& loc = source_location::current()) {
    std::ostringstream ostream;
    ostream << "file: "
            << loc.file_name()     << '('
            << loc.line()          << ':'
            << loc.column()        << ')' << ' ' << '`'
            << loc.function_name() << '`' << ':' << ' '
            << message             << '\n';
    return ostream.str();
}

bool Logger::append(const std::string &line) {
    std::ofstream log("log/" + Logger::fileName, std::ios::app);
    log << line;
    log.close();
    return false;
}

std::string Logger::getNameLog() {
    std::time_t t = std::time(0);
    auto time     = std::make_unique<std::tm*>(std::localtime(&t));

    std::string nameLog = "time-"                    +
                    std::to_string((*time)->tm_hour) + '`' +
                    std::to_string((*time)->tm_min)  + '`' +
                    std::to_string((*time)->tm_sec)  + ".log";
    return nameLog;
}
_LOG_END

#undef _LOG_BEGIN
#undef _LOG_END
#endif // _LOGGER_HPP_

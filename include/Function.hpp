#ifndef _FUNCTION_HPP_
#define _FUNCTION_HPP_
#include "Logger.hpp"

#include <unordered_map>
#include <cassert>
#include <string>

using namespace ::Log;

/// path to file - file/...
static std::unordered_map<std::string, size_t> functions = {
        {"nodes-6",       1 },
        {"nodes-12",      2 },
        {"nodes-24",      3 },
        {"study-nodes-5", 11},

        {"error",         0 }
    };

class Function
{
private:
    static size_t _func;

public:
    static void setFunction(const std::string& _path);

    static double lambda(size_t area);
    static double f(const Union::XY coord, const size_t area);

    static double firstBound (const Union::XY coord, const size_t type);
    static double secondBound(const Union::XY coord, const size_t type);
    static double thirdBound (const Union::XY coord, const size_t type);
};

size_t Function::_func = 0;


void Function::setFunction(const std::string& _path) {
    functions.contains(_path) ?
        Function::_func = functions[_path]  :
        Function::_func = functions["error"];
}

double Function::f(const Union::XY coord, const size_t area){
    switch (Function::_func) {
        /// file/nodes-6
        case 1: {
            switch (area) {
                case 0: return 2 * coord.x; break;
                default:
                    throw std::runtime_error("f - error");
            }
            break;
        }
        /// file/study-nodes-5
        case 11: {
            switch (area) {
                case 0: return -20; break;
                case 1: return  0;  break;
                default:
                    throw std::runtime_error("f - error");
            }
            break;
        }
        default:
            Logger::append(getLog("the number func do not exist"));
            throw std::runtime_error("number func - error");
    }
}

double Function::lambda(size_t area) {
    switch (Function::_func) {
        case 1: {
            switch (area) {
                case 0: return 1; break;
                default:
                    throw std::runtime_error("lambda - error");
            }
            break;
        }
        case 11: {
            switch (area) {
                case 0: return 10; break;
                case 1: return 1;  break;
                default:
                    throw std::runtime_error("lambda - error");
            }
            break;
        }
        default:
            Logger::append(getLog("the number func do not exist"));
            throw std::runtime_error("number func - error");

    }
}

double Function::firstBound(const Union::XY coord, const size_t type) {
    switch (Function::_func) {
        case 1: {
            switch (type) {
                case 0: return coord.x; break;
                default:
                    throw std::runtime_error("first - error");
            }
            break;
        }
        case 11: {
            switch (type) {
                case 0: return coord.y * coord.y; break;
                default:
                    throw std::runtime_error("first - error");
            }
            break;
        }
        default:
            Logger::append(getLog("the number func do not exist"));
            throw std::runtime_error("number func - error");

    }
}

double Function::secondBound(const Union::XY coord, const size_t type) {
    switch (Function::_func) {
        case 1: {
            switch (type) {
                case 0: return  1; break;
                case 1: return -1; break;
                case 2: return  0; break;
                default:
                    throw std::runtime_error("second - error");
            }
            break;
        }
        case 11: {
            switch (type) {
                case 0: return 20; break;
                case 1: return 0;  break;
                default:
                    throw std::runtime_error("second - error");
            }
            break;
        }
        default:
            Logger::append(getLog("the number func do not exist"));
            throw std::runtime_error("number func - error");

    }
}

double Function::thirdBound(const Union::XY coord, const size_t type) {
    switch (Function::_func) {
        case 1: {
            switch (type) {
                case 0: return coord.x; break;
                default:
                    throw std::runtime_error("third - error");
            }
            break;
        }
        case 11: {
            switch (type) {
                case 0: return 20 * coord.y - 27; break;
                default:
                    throw std::runtime_error("third - error");
            }
            break;
        }
        default:
            Logger::append(getLog("the number func do not exist"));
            throw std::runtime_error("number func - error");

    }
}

#endif /// _FUNCTION_HPP_
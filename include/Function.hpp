#ifndef _FUNCTION_HPP_
#define _FUNCTION_HPP_
#include "Logger.hpp"

#include <unordered_map>
#include <cassert>
#include <string>
#include <cmath>

using namespace ::Log;

/// path to file - file/...
static std::unordered_map<std::string, size_t> functions = {
        ///          unidentified        ///
        {"file/nodes-6",                1  },
        {"file/nodes-unknown",          2  },
        {"file/nodes-9",                3  },


        /// Rombeo test )==( Don't touch ///
        {"file/split-1/elements-2",     100},
        {"file/split-1/elements-4",     100},
        {"file/split-1/elements-8",     100},
        {"file/split-1/elements-16",    100},


        ///         Verified tests       ///
        {"file/light-test-u-const",     50 },   /// Non realisation
        {"file/light-test-u-func",      51 },


        /// my test (function: u =5x+2y) ///
        {"file/split-2/elements-2",     101},
        {"file/split-2/elements-4",     101},
        {"file/split-2/elements-8",     101},
        {"file/split-2/elements-16",    101},


        ///      tests from textbook     ///
        {"file/study-nodes-5",          11 },

        ///              error           ///
        {"error",                       0  }};


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

    #define SWITCH switch (area) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return 2 * coord.x; break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return -exp(coord.x + coord.y); break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return -20; break;
                    case 1:  return  0;  break;
                    default: std::exit(1);
                    BREAK


        case 50:
                    SWITCH
                    case 0:  return 0; break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return 0; break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 20 * coord.x + 20 * coord.y; break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 10 * coord.x + 4 * coord.y; break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist")
                    );  std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}

double Function::lambda(size_t area) {

    #define SWITCH switch (area) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return 1; break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return 2; break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return 10; break;
                    case 1:  return 1;  break;
                    default: std::exit(1);
                    BREAK


        case 50:
                    SWITCH
                    case 0:  return 1; break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return 1; break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 4; break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 2; break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist")
                    ); std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}

double Function::firstBound(const Union::XY coord, const size_t type) {

    #define SWITCH switch (type) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return coord.x; break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return exp(coord.x); break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return coord.y * coord.y; break;
                    default: std::exit(1);
                    BREAK


        case 50:
                    SWITCH
                    case 0:  return 1; break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return coord.x; break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 50 + 10 * coord.y; break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 5 * coord.x + 2; break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist")
                    ); std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}

double Function::secondBound(const Union::XY coord, const size_t type) {

    #define SWITCH switch (type) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return  1; break;
                    case 1:  return -1; break;
                    case 2:  return  0; break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return  2 * exp(coord.x + coord.y); break;
                    case 1:  return -2 * exp(coord.x + coord.y); break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return 20; break;
                    case 1:  return 0;  break;
                    default: std::exit(1);
                    BREAK


        case 50:
                    SWITCH
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return -40; break;
                    case 1:  return  40; break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return -10; break;
                    case 1:  return  10; break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist")
                    ); std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}

double Function::thirdBound(const Union::XY coord, const size_t type) {

    #define SWITCH switch (type) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return coord.x; break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return exp(coord.x + 1) - 2 * exp(coord.x + coord.y); break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return 20 * coord.y - 27; break;
                    default: std::exit(1);
                    BREAK


        case 50:
                    SWITCH
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 10 * coord.x + 2; break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 5 * coord.x + 8.8; break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist")
                    ); std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}
#endif /// _FUNCTION_HPP_
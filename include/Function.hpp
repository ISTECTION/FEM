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

        /// my test (lambda - function)  ///
        {"file/l-func",                 200},

        ///        test Poleleyka        ///
        {"file/poleleyka",              666},
        {"file/polina-top",           666-1},

        ///      tests from textbook     ///
        {"file/study-nodes-5",          11 },
        {"file/study-nodes-9",          12 },

        ///  Research order convergence  ///
        {"file/research/study-1",       11 },
        {"file/research/study-2",       11 },
        {"file/flower",                 22 },
        {"file/research/guard/test-1",  30 },
        {"file/research/guard/test-2",  30 },
        {"file/research/guard/test-3",  30 },


        ///         Test non boundary           ///
        {"file/split-2/elements-8-non-boards", 101},
        ///         Test non boundary           ///

        ///              error           ///
        {"error",                       0  }};


class Function
{
private:
    static size_t _func;

public:
    static void setFunction(const std::string& _path);
    static double f(const Union::XY coord, const size_t area);

    static double lambda   (const Union::XY coord, const size_t area);
    static double analitics(const Union::XY coord, const size_t area);

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

double Function::f(const Union::XY coord, const size_t area) {

    #define SWITCH switch (area) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return 2 * coord.x;                        break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return -exp(coord.x + coord.y);            break;
                    default: std::exit(1);
                    BREAK

        case 11:
                    SWITCH
                    case 0:  return -20;                                break;
                    case 1:  return  0;                                 break;
                    default: std::exit(1);
                    BREAK
        case 12:
                    SWITCH
                    case 0:  return 5 * coord.x + 30 * coord.y - 10;    break;
                    case 1:  return 0;                                  break;
                    default: std::exit(1);
                    BREAK
        case 22:
                    SWITCH
                    case 0:  return 4 * coord.x + 6 * coord.y - 7;      break;
                    default: std::exit(1);
                    BREAK
        case 30:
                    SWITCH
                    case 0:  return 2 * sin(coord.x);                   break;
                    default: std::exit(1);
                    BREAK

        case 50:
                    SWITCH
                    case 0:  return 0;                                  break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return 0;                                  break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 20 * coord.x + 20 * coord.y;        break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 10 * coord.x + 4 * coord.y;         break;
                    default: std::exit(1);
                    BREAK
        case 200:
                    SWITCH
                    case 0:  return 4 * coord.x - 2;                    break;
                    default: std::exit(1);
                    BREAK
        case 666:
        case 666-1:
                    SWITCH
                    case 0:
                        return 8 * coord.x + 12 * coord.y;              break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist (f)")
                    );  std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}


double Function::analitics(const Union::XY coord, const size_t area){

    #define SWITCH switch (area) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0: return coord.y * coord.y;                   break;
                    case 1: return 20 * coord.y - 19;                   break;
                    default: std::exit(1);
                    BREAK
        case 12:
                    SWITCH
                    case 0:
                    case 1: return coord.x + 6 * coord.y - 2;           break;
                    default: std::exit(1);
                    BREAK
        case 22:
                    SWITCH
                    case 0:  return 2 * coord.x + 3 * coord.y + 1;      break;
                    default: std::exit(1);
                    BREAK
        case 30:
                    SWITCH
                    case 0:  return sin(coord.x);                       break;
                    default: std::exit(1);
                    BREAK

        case 50:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return coord.x;                            break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 10 * coord.x + 10 * coord.y;        break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0: return 5 * coord.x + 2 * coord.y;           break;
                    default: std::exit(1);
                    BREAK
        case 200:
                    SWITCH
                    case 0: return 2 * coord.x;                         break;
                    default: std::exit(1);
                    BREAK
        case 666:
        case 666-1:
                    SWITCH
                    case 0: return 2 * coord.x + 3 * coord.y;           break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist (analitics)")
                    );  std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}

double Function::lambda(const Union::XY coord, const size_t area) {

    #define SWITCH switch (area) {
    #define BREAK  } break;

    switch (Function::_func) {
        case 1:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return 2;                                  break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return 10;                                 break;
                    case 1:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK
        case 12:
                    SWITCH
                    case 0:
                    case 1: return 1;                                   break;
                    default: std::exit(1);
                    BREAK
        case 22:
                    SWITCH
                    case 0:  return 3 * coord.y;                        break;
                    default: std::exit(1);
                    BREAK
        case 30:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK

        case 50:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 4;                                  break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 2;                                  break;
                    default: std::exit(1);
                    BREAK
        case 200:
                    SWITCH
                    case 0:  return coord.x;                            break;
                    default: std::exit(1);
                    BREAK
        case 666:
        case 666-1:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist (lambda)")
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
                    case 0:  return coord.x;                            break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return exp(coord.x);                       break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return coord.y * coord.y;                  break;
                    default: std::exit(1);
                    BREAK
        case 12:
                    SWITCH
                    case 0:  return 6 * coord.y + 2;                    break;
                    default: std::exit(1);
                    BREAK
        case 22:
                    SWITCH
                    case 0:  return 2 * coord.x + 4;                    break;
                    case 1:  return 3 * coord.y + 3;                    break;
                    case 2:  return 3 * coord.y + 9;                    break;
                    default: std::exit(1);
                    BREAK
       case 30:
                    SWITCH
                    case 0:  return sin(1.0);                   break;
                    case 1:  return sin(1.2);                   break;
                    default: std::exit(1);
                    BREAK

        case 50:
                    SWITCH
                    case 0:  return 1;                                  break;
                    default: std::exit(1);
                    BREAK
        case 51:
                    SWITCH
                    case 0:  return coord.x;                            break;
                    default: std::exit(1);
                    BREAK


        case 100:
                    SWITCH
                    case 0:  return 50 + 10 * coord.y;                  break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 5 * coord.x + 2;                    break;
                    default: std::exit(1);
                    BREAK
        case 200:
                    SWITCH
                    case 0:  return 2 * coord.x;                        break;
                    default: std::exit(1);
                    BREAK
        case 666:
                    SWITCH
                    case 0:  return 2 * coord.x;                        break;
                    default: std::exit(1);
                    BREAK
        case 666-1:
                    SWITCH
                    case 0:  return 3 * coord.y;                        break;
                    case 1:  return 2 * coord.x + 6;                    break;
                    case 2:  return 6 + 3 * coord.y;                    break;
                    case 3:  return 2 * coord.x;                        break;
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist (firstBound)")
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
                    case 0:  return  1;                                 break;
                    case 1:  return -1;                                 break;
                    case 2:  return  0;                                 break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:  return  2 * exp(coord.x + coord.y);        break;
                    case 1:  return -2 * exp(coord.x + coord.y);        break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return 20;                                 break;
                    case 1:  return 0;                                  break;
                    default: std::exit(1);
                    BREAK
        case 12:
                    SWITCH
                    case 0:  return -6;                                 break;
                    case 1:  return -1;                                 break;
                    case 2:  return  6;                                 break;
                    default: std::exit(1);
                    BREAK
        case 22:
                    SWITCH
                    case 0:  return -5 / sqrt(2) * 3 * coord.y;         break;
                    case 1:  return -1 / sqrt(2) * 3 * coord.y;         break;
                    case 2:  return  5 / sqrt(2) * 3 * coord.y;         break;
                    case 3:  return  1 / sqrt(2) * 3 * coord.y;         break;
                    default: std::exit(1);
                    BREAK
        case 30:
                    SWITCH
                    case 0:  return  exp(coord.x + coord.y);            break;
                    case 1:  return -exp(coord.x + coord.y);            break;
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
                    case 0:  return -40;                                break;
                    case 1:  return  40;                                break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return -10;                                break;
                    case 1:  return  10;                                break;
                    default: std::exit(1);
                    BREAK
        case 200:
                    SWITCH
                    case 0:  return -2 * coord.x;                       break;
                    case 1:  return  2 * coord.x;                       break;
                    default: std::exit(1);
                    BREAK
        case 666:
                    SWITCH
                    case 0:  return -2;                                 break;
                    case 1:  return  2;                                 break;
                    default: std::exit(1);
                    BREAK
        case 666-1:
                    SWITCH
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist (secondBound)")
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
                    case 0:  return coord.x;                            break;
                    default: std::exit(1);
                    BREAK
        case 3:
                    SWITCH
                    case 0:
                        return
                            exp(coord.x + 1) - 2 *
                            exp(coord.x + coord.y);                     break;
                    default: std::exit(1);
                    BREAK
        case 11:
                    SWITCH
                    case 0:  return 20 * coord.y - 27;                  break;
                    default: std::exit(1);
                    BREAK
        case 12:
                    SWITCH
                    case 0:  return 6 * coord.y + 2.1;                  break;
                    default: std::exit(1);
                    BREAK
        case 22:
                    SWITCH
                    case 0:
                        return (10 * coord.x + 9 * coord.y) / 5.0 + 13; break;
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
                    case 0:  return 10 * coord.x + 2;                   break;
                    default: std::exit(1);
                    BREAK
        case 101:
                    SWITCH
                    case 0:  return 5 * coord.x + 8.8;                  break;
                    default: std::exit(1);
                    BREAK
        case 200:
                    SWITCH
                    case 0:  return 2 * coord.x;                        break;
                    default: std::exit(1);
                    BREAK
        case 666:
                    SWITCH
                    case 0:  return 2 * coord.x + 7.5;                  break;
                    default: std::exit(1);
                    BREAK
        case 666-1:
                    SWITCH
                    default: std::exit(1);
                    BREAK
        default:
                    Logger::append(
                        getLog("the number func do not exist (thirdBound)")
                    ); std::exit(1);
    }
    #undef SWITCH
    #undef BREAK
}
#endif /// _FUNCTION_HPP_
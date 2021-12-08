#ifndef _FUNCTION_HPP_
#define _FUNCTION_HPP_
#include "Logger.hpp"
#include <cassert>

#define _FUNCTION_BEGIN namespace Function {
#define _FUNCTION_END                      }

_FUNCTION_BEGIN
#define STUDY_TEST1
using namespace ::Log;


/// ~~~~~~~~~~~~~ Тест №1 ~~~~~~~~~~~~~ ///
#ifdef TEST1
double f(const Union::XY coord, const size_t area) {

    switch (area)
    {
        case 0: return 2 * coord.x; break;
        default:
            Logger::append(getLog("The function do not exist"));
            return 0;
    }
}

double lambda(size_t area) {

    switch (area)
    {
        case 0: return 1; break;
        default:
            Logger::append(getLog("Lambda this area do not exist"));
            return 0;
    }
}

double firstBound(const Union::XY coord, const size_t type) {

    switch (type)
    {
        case 0: return coord.x; break;
        default:
            Logger::append(getLog("The first boundary do not exist"));
            return 0;
    }
}

double secondBound(const Union::XY coord, const size_t type) {

    switch (type)
    {
        case 0: return  1; break;
        case 1: return -1; break;
        case 2: return  0; break;
        default:
            Logger::append(getLog("The second boundary do not exist"));
            return 0;
    }
}

double thirdBound(const Union::XY coord, const size_t type) {

    switch (type)
    {
        case 0: return coord.x; break;
        default:
            Logger::append(getLog("The third boundary do not exist"));
            return 0;
    }
}
#endif

/// ~~~~~~~~~~~~~ Тест №2 ~~~~~~~~~~~~~ ///
#ifdef TEST2
#endif


/// ~~~~~~~~~~~~~ Тест №3 ~~~~~~~~~~~~~ ///
#ifdef TEST3
#endif





/// ~~~~~~~~~~~ Study test ~~~~~~~~~~~ ///
#ifdef STUDY_TEST1

double f(const Union::XY coord, const size_t area) {

    switch (area)
    {
        case 0: return -20; break;
        case 1: return  0;  break;
        default:
            Logger::append(getLog("The function do not exist"));
            return 0;
    }
}

double lambda(size_t area) {

    switch (area)
    {
        case 0: return 10; break;
        case 1: return 1;  break;
        default:
            Logger::append(getLog("Lambda this area do not exist"));
            return 0;
    }
}

double firstBound(const Union::XY coord, const size_t type) {

    switch (type)
    {
        case 0: return coord.y * coord.y; break;
        default:
            Logger::append(getLog("The first boundary do not exist"));
            return 0;
    }
}

double secondBound(const Union::XY coord, const size_t type) {

    switch (type)
    {
        case 0: return 20; break;
        case 1: return 0;  break;
        default:
            Logger::append(getLog("The second boundary do not exist"));
            return 0;
    }
}

double thirdBound(const Union::XY coord, const size_t type) {

    switch (type)
    {
        case 0: return 20 * coord.y - 27; break;
        default:
            Logger::append(getLog("The third boundary do not exist"));
            return 0;
    }
}
#endif

_FUNCTION_END

#undef _FUNCTION_BEGIN
#undef _FUNCTION_BEGIN
#endif /// _FUNCTION_HPP_
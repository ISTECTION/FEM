#ifndef _FUNCTION_HPP_
#define _FUNCTION_HPP_
#include "Logger.hpp"
#include <cassert>

#define _FUNCTION_BEGIN namespace Function {
#define _FUNCTION_END                      }

_FUNCTION_BEGIN
using namespace ::Log;

#define TEST1
///--------------------------------------------------------------------------///

#ifdef TEST1
double f(const Union::XY coord, const size_t area) {

    switch (area)
    {
        case 0: return 2 * coord.x; break;
        case 1: return 0;           break;

        default:
            Logger::append(getLog("The function do not exist"));
            return 0;
    }
}

double diffusionKoef(size_t area) {

    switch (area)
    {
        case 0: return 1; break;

        default:
            Logger::append(getLog("Diffusion this area do not exist"));
            return 0;
    }
}

double gammaKoef(size_t area) {

    switch (area)
    {
        case 0: return 2; break;

        default:
            Logger::append(getLog("Gamma this area do not exist"));
            return 0;
    }
}
#endif

///--------------------------------------------------------------------------///

#ifdef TEST2
double f(const Union::XY coord, const size_t area) {
    using namespace ::Log;

    const size_t NUMBER = 1;

    switch (NUMBER)
    {
        case 1:
        {
            if      (area == 0) return 0;
            else                return 0;
            break;
        }
        default:
            assert(Logger::append(getLog("The function does not exist")));
    }
}
#endif

///--------------------------------------------------------------------------///

_FUNCTION_END

#undef _FUNCTION_BEGIN
#undef _FUNCTION_BEGIN
#endif // _FUNCTION_HPP_
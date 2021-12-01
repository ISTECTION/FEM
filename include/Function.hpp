#ifndef _FUNCTION_HPP_
#define _FUNCTION_HPP_
#include "Logger.hpp"

#include <cassert>

#define _FUNCTION_BEGIN namespace Function {
#define _FUNCTION_END                      }

_FUNCTION_BEGIN

double f(const Union::XY coord, const size_t area) {
    using namespace ::Log;

    const size_t NUMBER = 1;

    switch (NUMBER)
    {
        case 1:
        {
            if      (area == 0) return 2 * coord.x;
            else if (area == 1) return 0;
            else                return 0;
            break;
        }
        default:
            assert(Logger::append(getLog("The function does not exist")));
    }
}
_FUNCTION_END

#undef _FUNCTION_BEGIN
#undef _FUNCTION_BEGIN
#endif // _FUNCTION_HPP_
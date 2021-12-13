#ifndef _UNION_HPP_
#define _UNION_HPP_
#include <vector>
#include <array>

#define _UNION_BEGIN namespace Union {
#define _UNION_END                   }

_UNION_BEGIN

struct XY {
    double x;
    double y;
    size_t area;
};

struct Material {
    double betta;
    double gamma;
};

struct Element {
    size_t area;
    std::array<size_t, 3> nodeIdx;
};

struct Boundary {
    size_t cond;
    size_t type;
    size_t area;
    std::array<size_t, 2> nodeIdx;
};

struct Param {
    size_t nodes;
    size_t elems;
    size_t areas;
    size_t conds;
};

_UNION_END
#undef _UNION_BEGIN
#undef _UNION_END
#endif /// _UNION_HPP_
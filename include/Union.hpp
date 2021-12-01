#ifndef _UNION_HPP_
#define _UNION_HPP_
#include <vector>
#include <array>

#define _UNION_BEGIN namespace Union {
#define _UNION_END                   }

_UNION_BEGIN

struct XY { double x, y; };

struct Material {
    double lambda;
    double betta;
    double gamma;
};

struct Element {
    size_t area;
    std::array<size_t, 3> nodeIdx;
};

struct Border {
    size_t area;
    size_t cond;
    size_t type;
    std::array<size_t, 2> bordIdx;
};

struct Param {
    size_t nodes;                                                               /// Количество узлов
    size_t elems;                                                               /// Количество елементов
    size_t areas;                                                               /// Количество областей
    size_t conds;                                                               /// Количество краевых условий
};

_UNION_END

#undef _UNION_BEGIN
#undef _UNION_END
#endif // _UNION_HPP_
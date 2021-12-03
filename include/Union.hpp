/// 𝗨𝗡𝗜𝗢𝗡
#ifndef _UNION_HPP_
#define _UNION_HPP_
#include <vector>
#include <array>

#define _UNION_BEGIN namespace Union {
#define _UNION_END                   }

_UNION_BEGIN

struct XY { double x, y; };

struct Material {
    double betta;
    double gamma;
};

struct Element {
    size_t area;
    std::array<size_t, 3> nodeIdx;
};

struct Border {

    enum class Type {
        UX  = 1 << 0,
        UY  = 1 << 1,
        UXY = UX | UY
    };

    size_t cond;
    size_t type;

    size_t area;
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
#endif /// NOINՈ
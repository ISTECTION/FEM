#ifndef _UNION_HPP_
#define _UNION_HPP_
#include <vector>
#include <array>

namespace Union {
    struct XY       { double x, y;         };   /// node coordinates
    struct Material { double betta, gamma; };   /// coefficient

    struct Element {
        size_t area;                            /// area element
        std::array<size_t, 3> nodeIdx;          /// node element
    };

    struct Boundary {                           /// 1. boundary condition
        size_t cond, type, area;                /// 2. type
        std::array<size_t, 2> nodeIdx;          /// 3. area
    };                                          /// 4. border nodes

    struct Param {
        size_t nodes;                           /// count nodes
        size_t elems;                           /// count elements
        size_t areas;                           /// count areas
        size_t conds;                           /// count boundary condition
    };
}
#endif /// _UNION_HPP_
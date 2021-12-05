#ifndef _OVERLOAD_HPP_
#define _OVERLOAD_HPP_
#include <iostream>
#include <vector>
#include <array>

namespace array {
    using x   =            std::array<double, 3>    ;
    using xxx = std::array<std::array<double, 3>, 3>;
}

array::xxx                                                                      /// Перегрузка сложения (размер array<3, 3>)
operator+ (const array::xxx& G, const array::xxx& M) {
    array::xxx _res;
    for (size_t i = 0; i < G.size(); i++)
    for (size_t j = 0; j < G.size(); j++)
        _res[i][j] = G[i][j] + M[i][j];
    return _res;
}
#endif // _OVERLOAD_HPP_
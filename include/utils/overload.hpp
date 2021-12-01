#ifndef _OVERLOAD_HPP_
#define _OVERLOAD_HPP_
#include <iostream>
#include <vector>
#include <array>

namespace array {
    using xxx = std::array<std::array<double, 3>, 3>;
}


array::xxx
operator+ (const array::xxx& G, const array::xxx& M) {

    array::xxx _res;
    for (size_t i = 0; i < G.size(); i++)
    for (size_t j = 0; j < G.size(); j++)
        _res[i][j] = G[i][j] + M[i][j];

    return _res;
}


// vector<vector<double>> &operator*= (vector<vector<double>> &vec, const double _coef) {

//     for (size_t i = 0; i < vec.size(); i++)
//         for (size_t j = 0; j < vec.size(); j++)
//             vec[i][j] *= _coef;

//     return vec;
// }

#endif // _OVERLOAD_HPP_
#ifndef _LIGHTWEIGHT_HPP_
#define _LIGHTWEIGHT_HPP_
#include <iostream>
#include <Union.hpp>

template<typename T>
void
print(const std::vector<T>& _vec) {
    for (size_t _pos = 0; _pos < _vec.size(); _pos++)
        std::cout << _vec[_pos] << ' ';
    std::cout << std::endl;
}

double
determinant(const std::array<Union::XY, 3>& xyElem) {
    return (
        (xyElem[1].x - xyElem[0].x) * (xyElem[2].y - xyElem[0].y) -
        (xyElem[1].y - xyElem[0].y) * (xyElem[2].x - xyElem[0].x)
    );
}
#endif // _LIGHTWEIGHT_HPP_
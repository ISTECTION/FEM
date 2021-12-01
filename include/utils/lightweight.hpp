#ifndef _LIGHTWEIGHT_HPP_
#define _LIGHTWEIGHT_HPP_
#include <iostream>
#include <vector>

template<typename T>
void
print(const std::vector<T>& _vec) {
    for (size_t _pos = 0; _pos < _vec.size(); _pos++)
        std::cout << _vec[_pos] << ' ';
    std::cout << std::endl;
}
#endif // _LIGHTWEIGHT_HPP_
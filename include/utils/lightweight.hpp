#ifndef _LIGHTWEIGHT_HPP_
#define _LIGHTWEIGHT_HPP_
#include "../Union.hpp"
#include <windows.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>

///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///
double
determinant(const std::array<Union::XY, 3>& elem) {
    return (
        (elem[1].x - elem[0].x) * (elem[2].y - elem[0].y) -
        (elem[1].y - elem[0].y) * (elem[2].x - elem[0].x)
    );
}
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~///

template<typename T>
void
print(const std::vector<T>& _vec) {
    for (size_t _pos = 0; _pos < _vec.size(); _pos++)
        std::cout << _vec[_pos] << ' ';
    std::cout << std::endl;
}

template<typename T>
void
printGlobal(const std::vector<std::vector<T>>& _global) {
    #define ENDLINE std::cout << '\n';
    ENDLINE
    for (size_t i = 0; i < _global.size(); i++) {
        for (size_t j = 0; j < _global.size(); j++)
            std::cout << _global[i][j] << '\n';
        ENDLINE
    }
    #undef ENDLINE
}

template<typename T>
void
printXXX(const std::array<std::array<T, 3>, 3>& A) {
    #define ENDLINE std::cout << '\n'
    std::cout << std::endl;
    for (size_t row = 0; row < 3; row++, ENDLINE)
    for (size_t column = 0; column < 3; column++)
        std::cout << A[row][column] << ' ';
    std::cout << std::endl;
    #undef ENDLINE
}


template<typename _It>
void
pretty(_It _beg, _It _end, const size_t _N = 3)  {
    
    if (_beg == _end) { std::cout << "Empty" << '\n'; }
    else {

        #define EPSILON 0.0000000001
        #define EQUAL(a, b) (abs((a) - (b)) < EPSILON)
        SetConsoleOutputCP(65001);

        std::ostringstream osstr, main;
        std::vector<std::string> strs;
        size_t mwidth = 0;

        for(auto iter = _beg; iter != _end; iter++) {
            auto _row = *iter;
            for(auto itr = _row.begin(); itr != _row.end(); itr++) {
                double term = *itr;
                if (EQUAL(*itr, 0)) term = 0;
                osstr << term;
                std::string str = osstr.str();
                osstr.str(std::string());
                strs.push_back(str);
                size_t nl = str.size();
                mwidth = nl > mwidth ? nl : mwidth;
            }
        }

        size_t midwidth = (mwidth * _N) + ((_N + 1) << 1);
        main << "\u250c\u2500" << std::setw(midwidth - 2)
             << "" << "\u2500\u2510";
        for (size_t i = 0; i < _N; i++) {
            main << std::endl;
            main << "\u2502";
            for (size_t j = 0; j < _N; j++) {
                std::string& str = strs[i + (j * 3)];
                int wlen = (str.size() + mwidth + 1) >> 1;
                main << "  ";
                main << std::setw(wlen);
                main << str;
                main << std::setw(mwidth - wlen) << "";
            }
            main << "  \u2502";
        }
        main << std::endl << "\u2514\u2500" << std::setw(midwidth - 2)
             << ""        << "\u2500\u2518" << std::endl;
        std::cout << main.str();
        #undef EPSILON
        #undef EQUAL
    }
}

template <typename T>
void
pretty(const std::vector<std::vector<T>>& _vec) {
        pretty(_vec.begin(), _vec.end(), _vec.size());
}

template <typename T>
void
pretty(const std::array<std::array<T, 3>, 3>& _arr) {
        pretty(_arr.begin(), _arr.end());
}

// template<typename T>
// void
// pretty(const std::array<std::array<T, 3>, 3>& A)  {
//     if (A.empty()) std::cout << "Empty";

//     #define EPSILON 0.0000000001
//     #define EQUAL(a, b) (abs((a) - (b)) < EPSILON)
//     SetConsoleOutputCP(65001);

//     std::ostringstream osstr, main;
//     std::vector<std::string> strs;
//     size_t mwidth = 0;
//     for (size_t j = 0; j < 3; ++j) {
//         for (size_t i = 0; i < 3; ++i) {
//             double term = A[i][j];
//             if (EQUAL(term, 0))
//                 term = 0;

//             osstr << term;
//             std::string str = osstr.str();
//             osstr.str(std::string());
//             strs.push_back(str);
//             size_t nl = str.size();
//             mwidth = nl > mwidth ? nl : mwidth;
//         }
//     }
//     size_t midwidth = (mwidth * 3) + ((3 + 1) << 1);
//     main << "\u250c\u2500" << std::setw(midwidth - 2) << "" << "\u2500\u2510";
//     for (size_t i = 0; i < 3; i++) {
//         main << std::endl;
//         main << "\u2502";
//         for (size_t j = 0; j < 3; j++) {
//             std::string& str = strs[i + (j * 3)];
//             int wlen = (str.size() + mwidth + 1) >> 1;
//             main << "  ";
//             main << std::setw(wlen);
//             main << str;
//             main << std::setw(mwidth - wlen) << "";
//         }
//         main << "  \u2502";
//     }
//     main << std::endl << "\u2514\u2500" << std::setw(midwidth - 2)
//          << ""        << "\u2500\u2518" << std::endl;
//     std::cout << main.str();
//     #undef EPSILON
//     #undef EQUAL
// }
#endif // _LIGHTWEIGHT_HPP_
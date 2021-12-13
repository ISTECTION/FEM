#ifndef _LIGHTWEIGHT_HPP_
#define _LIGHTWEIGHT_HPP_
#include "../Union.hpp"
#include "friendly.hpp"
#include <windows.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <cmath>

/// ~~~~~~~~~~~~~~~~~~ Linear algebra ~~~~~~~~~~~~~~~~~~ ///
double                                                                          /// Функция вычисляющая детерминант
determinant(const std::array<Union::XY, 3>& elem) {
    return (
        (elem[1].x - elem[0].x) * (elem[2].y - elem[0].y) -
        (elem[1].y - elem[0].y) * (elem[2].x - elem[0].x)
    );
}

double                                                                          /// Функция вычисляет расстояние
edgeLength(const std::array<Union::XY, 2>& elem) {                              /// между 2-я координатами
    return (
        sqrt (
            pow(elem[1].x - elem[0].x ,2) +
            pow(elem[1].y - elem[0].y, 2)
        )
    );
}
/// ~~~~~~~~~~~~~~~~~~ Linear algebra ~~~~~~~~~~~~~~~~~~ ///

template<size_t _Size, typename T>                                              /// Вывод array<_Size, _Size> для матриц M, G
void printXXX(const std::array<std::array<T, _Size>, _Size>& A) {               /// и локальной матрицы A
    #define ENDLINE std::cout << '\n'
    std::cout << std::endl;
    for (size_t row = 0; row < _Size; row++, ENDLINE)
    for (size_t column = 0; column < _Size; column++)
        std::cout << A[row][column] << ' ';
    std::cout << std::endl;
    #undef ENDLINE
}



template <typename T>
void print(const std::vector<T>& _vec, std::streamsize count = 0) {             /// Примитивный вывод вектора
    std::ostringstream ostream;
    if (count) {
        ostream.setf(std::ios::fixed);
        ostream.precision(count);
    }
    ostream << "[ ";
    for (size_t i = 0; i < _vec.size(); i++)
        ostream << _vec[i] << " ";
    ostream << "]\n";
    std::cout << ostream.str();
}

template<typename _It>                                                          /// Красивый вывод, для двумерных:
void pretty(_It _beg, _It _end, const size_t _n)  {                             /// vector, array, с указанием итератора
    if (_beg == _end) { std::cout << "Empty" << '\n'; }                         /// начала и конца и размера строки
    else {
        #define EPSILON 0.0000000001
        #define EQUAL(a, b) (fabs((a) - (b)) < EPSILON)
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
        size_t midwidth = (mwidth * _n) + ((_n + 1) << 1);
        main << "\u250c\u2500" << std::setw(midwidth - 2)
             << "" << "\u2500\u2510";
        for (size_t i = 0; i < _n; i++) {
            main << std::endl;
            main << "\u2502";
            for (size_t j = 0; j < _n; j++) {
                std::string& str = strs[j + (i * _n)];
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

template<typename _It>                                                          /// Красивый вывод, для одномерных:
void pretty(_It _beg, _It _end)  {                                              /// vector, array, с указанием итератора
    if (_beg == _end) { std::cout << "Empty" << '\n'; }                         /// начала и конца
    else {
        #define EPSILON 0.0000000001
        #define EQUAL(a, b) (fabs((a) - (b)) < EPSILON)
        SetConsoleOutputCP(65001);

        std::ostringstream osstr, main;
        std::vector<std::string> strs;
        size_t mwidth = 0;

        size_t _count = 0;
        for(auto iter = _beg; iter != _end; iter++) {
            double term = *iter;
            if (EQUAL(term, 0)) term = 0;
            osstr << term;
            std::string str = osstr.str();
            osstr.str(std::string());
            strs.push_back(str);
            size_t nl = str.size();
            mwidth = nl > mwidth ? nl : mwidth;
            _count++;
        }
        size_t midwidth = (mwidth * _count) + ((_count + 1) << 1);
        main << "\u250c\u2500" << std::setw(midwidth - 2) << ""
             << "\u2500\u2510" << std::endl << "\u2502";
        for (size_t i = 0; i < _count; i++) {
            std::string& str = strs[i];
            int wlen = (str.size() + mwidth + 1) >> 1;
            main << "  ";
            main << std::setw(wlen);
            main << str;
            main << std::setw(mwidth - wlen) << "";
        }
        main << "  \u2502" << std::endl
             << "\u2514\u2500" << std::setw(midwidth - 2)
             << ""         << "\u2500\u2518" << std::endl;
        std::cout << main.str();
        #undef EPSILON
        #undef EQUAL
    }
}

void prettyG(const Sparse& sparse) {                                            /// матрицы, которая автоматически передаёт
    std::vector<std::vector<double>> gm;                                        /// параметры методу написаному выше
    gm.resize(sparse.di.size(),
        std::vector<double>(sparse.di.size()));

    size_t pos = 0;
    for(size_t i = 0; i < sparse.di.size(); i++) {
        gm[i][i] = sparse.di[i];

        for(size_t j = sparse.ig[i]; j < sparse.ig[i + 1]; j++) {
            gm[i][sparse.jg[pos]] = sparse.gg[pos];
            pos++;
        }
    }
    pretty(gm.begin(), gm.end(), sparse.di.size());
}

template <typename T>                                                           /// Шаблон функции для вывода красивого вектора
void pretty(const std::vector<T>& _vec) {                                       /// которая автоматически передаёт параметры
        pretty(_vec.begin(), _vec.end());                                       /// одномерного вектора методу написаному выше
}

template <size_t _Size, typename T>                                             /// Шаблон функции для вывода красивого вектора
void pretty(const std::array<T, _Size>& _arr) {                                 /// которая автоматически передаёт параметры
        pretty(_arr.begin(), _arr.end());                                       /// одномерного массива методу написаному выше
}

template <typename T>                                                           /// Шаблон функции для вывода красивой матрицы
void pretty(const std::vector<std::vector<T>>& _vec) {                          /// которая автоматически передаёт параметры
        pretty(_vec.begin(), _vec.end(), _vec.size());                          /// двумерного вектора методу написаному выше
}

template <size_t _Size, typename T>                                             /// Шаблон функции для вывода красивой матрицы
void pretty(const std::array<std::array<T, _Size>, _Size>& _arr) {              /// которая автоматически передаёт параметры
        pretty(_arr.begin(), _arr.end(), _Size);                                /// двумерного массива методу написаному выше
}

namespace Output {
    struct Write {                                                              /// Структура параметров для записи файлов:
        std::streamsize precision;                                              /// Количество знаков после запятой
        char            separator;                                              /// Разделитель
    };

    template<typename T>
    void write(                                                                 /// Функция записи вектора в файл, которая получает:
            const std::filesystem::path& _path,                                 /// Путь
            const std::vector<T>& _vec,                                         /// Вектор
            const Write _option = { 8, ' ' }) {                                 /// Структуру параметров

        std::ofstream fout(_path);
        fout.precision(_option.precision);
        for (size_t _pos = 0; _pos < _vec.size(); _pos++)
            fout << _vec[_pos] << _option.separator;
        fout.close();
    }
};
#endif // _LIGHTWEIGHT_HPP_
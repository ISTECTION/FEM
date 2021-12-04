#ifndef _DATA_HPP_
#define _DATA_HPP_
#include "../Logger.hpp"
#include <filesystem>
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

#define _SYMMETRIC_BEG namespace Symmetric {
#define _SYMMETRIC_END                     }

_SYMMETRIC_BEG                                                                  /// Пространство имён симметричной матрицы

struct Param {
    size_t n;                                                                   /// n - Размерность матрицы
    double epsilon;                                                             /// epsilon  - Точность решения СЛАУ
    size_t max_iter;                                                            /// max_iter - MAX количество итераций
};

enum class Cond {
    NONE,                                                                       /// Без предобусловливания
    DIAGONAL,                                                                   /// С диагональным предобусловливанием
    HOLLESKY                                                                    /// С предобусловливанием Холесского
};

template <class T>
class Data
{
protected:
    Param param;                                                                /// Структура входных данных файла kuslau.txt
    std::vector<size_t> ig;                                                     /// Указатели начала строк
    std::vector<size_t> jg;                                                     /// Номера столбцов
    std::vector<T> di;                                                          /// Диагональные элементы
    std::vector<T> gg;                                                          /// Внедиагональные элементы
    std::vector<T> b;                                                           /// Вектор правой части (Глобальный из МКЭ)
    std::vector<T> x;                                                           /// Вектор решения

    std::vector<T> di_l;                                                        /// Диагональные    элементы LU-разложения
    std::vector<T> gg_l;                                                        /// Внедиагональные элементы LU-разложения
    std::vector<T> y;                                                           /// Вектор y - для прямого хода
    size_t iter{ 0 };                                                           /// Количество итераций

public:
    Data(std::filesystem::path _path) { assert(loadData(_path)); }
    ~Data() { }

    std::vector<T>& getX() const { return x;    }                               /// Получить вектор решений
    size_t  getIteration() const { return iter; }                               /// Получить количество итераций
    void printX(std::streamsize count = 0) const;                               /// Вывести вектор решений

    void convertToLU();                                                         /// LL^T-разложение
    std::vector<T> normal (std::vector<T> b);                                   /// Прямой   ход
    std::vector<T> reverse(std::vector<T> y);                                   /// Обратный ход
    std::vector<T> mult(const std::vector<T>& _vec);                            /// Умножение на вектор

private:
    bool loadData(std::filesystem::path _path);                                 /// Функция считывания входных файлов
};

template <class T>
void Data<T>::convertToLU() {
    di_l = di;
    gg_l = gg;

    for (size_t i = 0; i < param.n; i++) {
        T sum_diag = 0;
        for (size_t j = ig[i]; j < ig[i + 1] ; j++) {
            T sum = 0;
            int jk = ig[jg[j]];
            int ik = ig[i];
            while ((ik < j) && (jk < ig[jg[j] + 1]))
            {
                int l = jg[jk] - jg[ik];
                if (l == 0) {
                    sum += gg_l[jk] * gg_l[ik];
                    ik++; jk++;
                }
                jk += (l < 0);
                ik += (l > 0);
            }
            gg_l[j]  -= sum;
            gg_l[j]  /= di_l[jg[j]];
            sum_diag += gg_l[j] * gg_l[j];
        }
        di_l[i] -= sum_diag;
        di_l[i] = sqrt(abs(di_l[i]));
    }
}

template <class T>
std::vector<T> Data<T>::normal(std::vector<T> b) {
    for (size_t i = 0; i < param.n; i++) {
        for (size_t j = ig[i]; j < ig[i + 1]; j++)
            b[i] -= gg_l[j] * b[jg[j]];

        b[i] = b[i] / di_l[i];
    }
    return b;
}

template <class T>
std::vector<T> Data<T>::reverse(std::vector<T> x) {
    for (int j = param.n - 1; j >= 0; j--) {
        x[j] = x[j] / di_l[j];

        for (size_t i = ig[j]; i < ig[j + 1]; i++)
            x[jg[i]] -= gg_l[i] * x[j];
    }
    return x;
}

template <class T>
std::vector<T> Data<T>::mult(const std::vector<T>& _vec) {
    std::vector<T> pr(_vec.size());

    int jj = 0;
    for (size_t i = 0; i < _vec.size(); i++) {
        pr[i] = di[i] * _vec[i];

        for (size_t j = ig[i]; j < ig[i + 1]; j++, jj++) {
            pr[i] += gg[jj] * _vec[jg[jj]];
            pr[jg[jj]] += gg[jj] * _vec[i];
        }
    }
    return pr;
}

template <class T>
void Data<T>::printX(std::streamsize count) const {
    std::ostringstream ostream;
    if (count) {
        ostream.setf(std::ios::fixed);
        ostream.precision(count);
    }
    ostream << "[ ";
    for (size_t i = 0; i < x.size(); i++)
        ostream << x[i] << " ";
    ostream << "]";
    std::cout << ostream.str();
}

template <typename T>
bool read(std::filesystem::path _path, std::vector<T>& _vec) {
    using namespace ::Log;
    std::ifstream fin(_path);
    if (not
        is_open(fin, "Error - " + _path.filename().string()))
        return false;
    for (size_t i = 0; i < _vec.size(); i++)
            fin >> _vec[i];
    fin.close(); return true;
}

template <class T>
bool Data<T>::loadData(std::filesystem::path _path) {
    using namespace ::Log;
    std::ifstream fin(_path / "kuslau.txt");
    if (not is_open(fin, "Error - kuslau.txt"))
        return false;
    fin >> param.n
        >> param.epsilon
        >> param.max_iter;
    fin.close();

    bool is_cor {  true  };
    ig.resize(param.n + 1);                                                     /// Выделение памяти для вектора хранящий указатели начала строк

    is_cor &= read(_path / "ig.txt", ig);

    gg.resize(ig.back());                                                       /// Выделение памяти под вектор хранящий нижний треугольник матрицы
    jg.resize(ig.back());                                                       /// Выделение памяти под вектор хранящий индексы столбцов
    di.resize( param.n );                                                         /// Выделение памяти под вектор хранящий диагональные элементы

    b.resize (param.n);                                                         /// Выделение памяти для элементов правой части
    x.resize (param.n);                                                         /// Выделение памяти для вектора решений СЛАУ

    is_cor &= read(_path / "gg.txt", gg);
    is_cor &= read(_path / "di.txt", di);
    is_cor &= read(_path / "jg.txt", jg);
    is_cor &= read(_path / "gb.txt", b);                                         /// Глобальная матрица b
    return is_cor;
}
_SYMMETRIC_END
#undef _SYMMETRIC_DEGIN
#undef _SYMMETRIC_END
#endif /// _DATA_HPP_
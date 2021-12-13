#ifndef _DATA_HPP_
#define _DATA_HPP_
#include "../utils/friendly.hpp"
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

_SYMMETRIC_BEG

struct Param {
    size_t n;
    double epsilon;
    size_t max_iter;
};

enum class Cond {
    NONE,
    DIAGONAL,
    HOLLESKY
};

template <class T>
class Data
{
protected:
    Param param;
    std::vector<size_t> ig;
    std::vector<size_t> jg;
    std::vector<T> di;
    std::vector<T> gg;
    std::vector<T> b;
    std::vector<T> x;

    std::vector<T> di_l;
    std::vector<T> gg_l;
    std::vector<T> y;
    size_t iter{ 0 };

public:
    Data(std::filesystem::path _path) { assert(loadData(_path)); }
    Data(Friendly* _friend, size_t _n, T _eps, size_t _max_iter) {
        param.n        = _n;
        param.epsilon  = _eps;
        param.max_iter = _max_iter;

        ig = _friend->ig;
        jg = _friend->jg;
        gg = _friend->gg;
        di = _friend->di;
        b  = _friend->gb;

        x.resize(_n);
        delete _friend;
    }
    ~Data() { }

    std::vector<T>& getX() const { return x;    }
    size_t  getIteration() const { return iter; }
    void printX(std::streamsize count = 0) const;

    void convertToLU();
    std::vector<T> normal (std::vector<T> b);
    std::vector<T> reverse(std::vector<T> y);
    std::vector<T> mult(const std::vector<T>& _vec);

private:
    bool loadData(std::filesystem::path _path);
};

template <class T>
void Data<T>::convertToLU() {
    di_l = di;
    gg_l = gg;

    for (size_t i = 0; i < param.n; i++) {
        T sum_diag = 0;
        for (size_t j = ig[i]; j < ig[i + 1] ; j++) {
            T sum = 0;
            size_t jk = ig[jg[j]];
            size_t ik = ig[i];
            while ((ik < j) && (jk < ig[jg[j] + 1]))
            {
                size_t l = jg[jk] - jg[ik];
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
        di_l[i] = sqrt(fabs(di_l[i]));
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
    ostream << '\n';
    if (count) {
        ostream.setf(std::ios::fixed);
        ostream.precision(count);
    }
    ostream << "[ ";
    for (size_t i = 0; i < x.size(); i++)
        ostream << x[i] << " ";
    ostream << "]\n";
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
    ig.resize(param.n + 1);

    is_cor &= read(_path / "ig.txt", ig);

    gg.resize(ig.back());
    jg.resize(ig.back());
    di.resize( param.n );

    b.resize (param.n);
    x.resize (param.n);

    is_cor &= read(_path / "gg.txt", gg);
    is_cor &= read(_path / "di.txt", di);
    is_cor &= read(_path / "jg.txt", jg);
    is_cor &= read(_path / "gb.txt", b);
    return is_cor;
}
_SYMMETRIC_END
#undef _SYMMETRIC_DEGIN
#undef _SYMMETRIC_END
#endif /// _DATA_HPP_
#ifndef _LOS_FUNCTION_HPP_
#define _LOS_FUNCTION_HPP_

#include <numeric>
#include <vector>
#include <cmath>

template <typename T>
inline void printLog(size_t _iter, T _eps) {
    std::cout << "Iteration = "   << std::fixed      << _iter << "\t\t" <<
                 "Discrepancy = " << std::scientific << _eps  << std::endl;
}

struct Scalar {
    template <typename T>
    T operator() (const std::vector<T>& _v1, const std::vector<T>& _v2) {
        T _res = 0;
        for (size_t i = 0; i < _v1.size(); i++)
            _res += _v1[i] * _v2[i];
        return _res;
    }
};

struct Norm {
    template <typename T>
    T operator() (const std::vector<T>& _v) {
        return sqrt(std::accumulate(_v.begin(), _v.end(), 0.0,
            [] (double _S, const double &_El) { return _S + _El * _El; }));
    }
};

template <typename T>
std::vector<T> operator* (std::vector<T> _v1, const std::vector<T>& _v2) {
    for (size_t i = 0; i < _v1.size(); i++)
        _v1[i] *= _v2[i];
    return _v1;
};

template <typename T>
std::vector<T> operator- (std::vector<T> _v1, const std::vector<T>& _v2) {
    for (size_t i = 0; i < _v1.size(); i++)
        _v1[i] -= _v2[i];
    return _v1;
};

template <typename T>
std::vector<T> operator+ (std::vector<T> _v1, const std::vector<T>& _v2) {
    for (size_t i = 0; i < _v1.size(); i++)
        _v1[i] += _v2[i];
    return _v1;
};

template <typename T>
std::vector<T> operator* (T _alpha, std::vector<T> _v1) {
    for (size_t i = 0; i < _v1.size(); i++)
        _v1[i] *= _alpha;
    return _v1;
};
#endif // _LOS_FUNCTION_HPP_
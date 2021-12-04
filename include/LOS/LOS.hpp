#ifndef _LOS_HPP_
#define _LOS_HPP_
#include "Data.hpp"

#include "LOS_Function.hpp"
#include "../Logger.hpp"

using namespace Symmetric;

#define LOGGER if (isLog) \
                    printLog(this->iter, eps);

template <class T>
class LOS : public Data<T>
{
public:
     LOS(std::filesystem::path _path) : Data<T>(_path) { }
    ~LOS() { }

    void solve(Cond _cond, bool isLog = true);
private:
    void none    (bool);
    void diagonal(bool);
    void hollesky(bool);
};

template <class T>
void LOS<T>::solve(Cond _cond, bool isLog) {
    using namespace ::Log;

    switch (_cond) {
        case Cond::NONE:     none(  isLog  ); break;
        case Cond::DIAGONAL: diagonal(isLog); break;
        case Cond::HOLLESKY: hollesky(isLog); break;
        default:
            Logger::append(getLog("this conditional non exist"));
            std::exit(1);
    }
}

template <class T>
void LOS<T>::none(bool isLog) {
    std::vector<T> r (this->param.n),
                   z (this->param.n),
                   p (this->param.n),
                   Ar(this->param.n);

    r = this->b - this->mult(this->x);
    z = r;
    p = this->mult(z);

    T alpha, betta, eps;
    do {
        betta   = scalar(p, p);
        alpha   = scalar(p, r) / betta;
        this->x = this->x + alpha * z;
        r       = r - alpha * p;
        Ar      = this->mult(r);
        betta   = scalar(p, Ar) / betta;
        z       = r  - betta * z;
        p       = Ar - betta * p;
        eps     = scalar(r, r);

        this->iter++;
        LOGGER

    } while (this->iter < this->param.max_iter
                 && eps > this->param.epsilon );
}

template <class T>
void LOS<T>::diagonal(bool isLog) {
    std::vector<T> r (this->param.n),
                   z (this->param.n),
                   p (this->param.n),
                   Ar(this->param.n);

    std::vector<T> L(this->param.n, 1);
    for (size_t i = 0; i < L.size(); i++)
        L[i] /= sqrt(this->di[i]);

    r = L * (this->b - this->mult(this->x));
    z = L * r;
    p = L * this->mult(z);

    T alpha, betta, eps;
    do {
        betta   = scalar(p, p);
        alpha   = scalar(p, r) / betta;
        this->x = this->x + alpha * z;
        r       = r - alpha * p;
        betta   = scalar(p, L * this->mult(L * r)) / betta;
        z       = L * r - betta * z;
        p       = L * this->mult(L * r) - betta * p;
        eps     = scalar(r, r);

        this->iter++;
        LOGGER

    } while (this->iter < this->param.max_iter
                 && eps > this->param.epsilon );
}

template <class T>
void LOS<T>::hollesky(bool isLog) {
    std::vector<T> r  (this->param.n),
                   z  (this->param.n),
                   p  (this->param.n),
                   Ar (this->param.n),
                   LAU(this->param.n);

    this->convertToLU();
    r = this->normal(this->b - this->mult(this->x));
    z = this->reverse(r);
    p = this->normal(this->mult(z));

    T alpha, betta, eps;
    do {
        betta   = scalar(p, p);
        alpha   = scalar(p, r) / betta;
        this->x = this->x + alpha * z;
        r       = r - alpha * p;
        LAU     = this->normal(this->mult(this->reverse(r)));
        betta   = scalar(p, LAU) / betta;
        z       = this->reverse(r) - betta * z;
        p       = LAU - betta * p;
        eps     = scalar(r, r);

        this->iter++;
        LOGGER
    }
    while(
        this->iter < this->param.max_iter
            && eps > this->param.epsilon );
}
#undef LOGGER
#endif /// _LOS_HPP_
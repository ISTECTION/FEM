#ifndef _FEM_HPP_
#define _FEM_HPP_
#include "indicators/indicators.hpp"
#include "utils/lightweight.hpp"
#include "utils/overload.hpp"
#include "utils/friendly.hpp"
#include "nlohmann/json.hpp"
#include "Function.hpp"
#include "LOS/LOS.hpp"
#include "Logger.hpp"
#include "Union.hpp"

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <set>

class FEM
{
private:
    Union::Param _size;

    std::vector<Union::XY>       nodes;
    std::vector<Union::Element>  elems;
    std::vector<Union::Boundary> boundarys;
    std::vector<Union::Material> materials;

    std::vector<double> gb;
    std::vector<double> gg;
    std::vector<double> di;
    std::vector<size_t> ig;
    std::vector<size_t> jg;

    bool _debugging;
public:
    FEM(std::filesystem::path _path, bool debugging)
            : _debugging(debugging) {

        assert(readJson(_path));
        portrait(true);
        global();
        boundaryCondition();
    }

    void startLOS(const std::filesystem::path& _out) {
        using ::Cond::HOLLESKY;
        using ::Cond::DIAGONAL;
        using ::Cond::NONE;

        const double _eps = 1E-20;
        const double _itr = 10000;
        Cond _cond = DIAGONAL;

        LOS<double> _LOS (getDate(), getNodes(), _eps, _itr);
        _LOS.solve(_cond, true);
        _z = std::move(_LOS.getX());
    }

    double getValue   (double, double) const;
private:
    void global();
    void resize();

    array::x   localB(const std::array<Union::XY, 3>&, size_t) const;
    array::xxx localA(const std::array<Union::XY, 3>&, size_t) const;

    array::xxx G(const std::array<Union::XY, 3>&, size_t) const;
    array::xxx M(const std::array<Union::XY, 3>&, size_t) const;

    bool readFile(const std::filesystem::path&  );
    bool readJson(const std::filesystem::path&  );
    void portrait(const bool isWriteList = false);

    void boundaryCondition();
    void first (const Union::Boundary& bound);
    void second(const Union::Boundary& bound);
    void third (const Union::Boundary& bound);

    template<size_t N,
        typename _Struct>
        void loc_A_to_global(
            const std::array<std::array<double, N>, N>&,
            const _Struct&
        );

    template<size_t N,
        typename _Struct>
        void loc_b_to_global(
            const std::array<double, N>&,
            const _Struct&
        );
};

void FEM::portrait(const bool isWriteList) {

    const size_t N {    _size.nodes     };
    std::vector<std::set<size_t>> list(N);

    for (size_t el = 0; el < _size.elems; el++)
    for (size_t point = 0; point < 3; point++) {
        for (size_t i = point + 1; i < 3; i++) {
            size_t idx1 = { elems[el].nodeIdx[point] };
            size_t idx2 = { elems[el].nodeIdx[  i  ] };
            idx1 > idx2 ?
                list[idx1].insert(idx2) :
                list[idx2].insert(idx1) ;
        }
    }

    for (size_t i = 2; i < ig.size(); i++)
        ig[i] = ig[i - 1] + list[i - 1].size();

    jg.resize(ig[N]);
    gg.resize(ig[N]);

    for (size_t index = 0, i = 1; i < list.size(); i++)
    for (size_t value : list[i])
        jg[index++] = value;
}

void FEM::global() {
    std::array<Union::XY, 3> coords;
    for (size_t i = 0; i < _size.elems; i++) {
        for (size_t j = 0; j < 3; j++) {
            size_t point = elems[i].nodeIdx[j];
            coords[j].x = nodes[point].x;
            coords[j].y = nodes[point].y;
        }
        array::x   local_b = localB(coords, elems[i].area);
        array::xxx local_A = localA(coords, elems[i].area);

        loc_A_to_global<3>(local_A, elems[i]);
        loc_b_to_global<3>(local_b, elems[i]);
    }
}

template<size_t N, typename _Struct>
void FEM::loc_A_to_global(
        const std::array<std::array<double, N>, N>& locA,
        const _Struct& elem) {

    using            ::std::vector;
    using iterator = ::std::vector<size_t>::iterator;

    for (size_t i = 0; i < N; i++) {
        di[elem.nodeIdx[i]] += locA[i][i];

        for (size_t j = 0; j < i; j++) {
            size_t a = elem.nodeIdx[i];
            size_t b = elem.nodeIdx[j];
            if (a < b) std::swap(a, b);

            iterator _beg = jg.begin() + ig[a];
            iterator _end = jg.begin() + ig[a + 1];

            auto _itr = std::lower_bound(_beg, _end, b);
            auto _idx = _itr - jg.begin();
            gg[_idx] += locA[i][j];
        }
    }
}

template<size_t N, typename _Struct>
void FEM::loc_b_to_global(
        const std::array<double, N>& loc_b,
        const _Struct& elem) {

    for (size_t i = 0; i < N; i++)
        gb[elem.nodeIdx[i]] += loc_b[i];
}

array::x
FEM::localB(const std::array<Union::XY, 3>& elem, size_t area) const {
    std::array<double, 3> function {
        Function::f(elem[0], area),
        Function::f(elem[1], area),
        Function::f(elem[2], area)
    };

    double det_D = fabs(determinant(elem));
    double _koef = det_D / 24;

    return {
        _koef * (2 * function[0] + function[1] + function[2]),
        _koef * (2 * function[1] + function[0] + function[2]),
        _koef * (2 * function[2] + function[0] + function[1])
    };
}

array::xxx
FEM::localA(const std::array<Union::XY, 3>& elem, size_t area) const {
    std::array<std::array<double, 3>, 3> G = FEM::G(elem, area);
    std::array<std::array<double, 3>, 3> M = FEM::M(elem, area);
    std::array<std::array<double, 3>, 3> A = G + M;
    return A;
}

array::xxx
FEM::G(const std::array<Union::XY, 3>& elem, size_t area) const {
    double det   = fabs(determinant(elem));
    double _koef = (
            Function::lambda(elem[0], area) +
            Function::lambda(elem[1], area) +
            Function::lambda(elem[2], area)
    ) / (det * 6);

    std::array<std::array<double, 3>, 3> G;
    std::array<std::array<double, 2>, 3> a {
            elem[1].y - elem[2].y,
            elem[2].x - elem[1].x,

            elem[2].y - elem[0].y,
            elem[0].x - elem[2].x,

            elem[0].y - elem[1].y,
            elem[1].x - elem[0].x
    };

    for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
        G[i][j] = _koef * (
            a[i][0] * a[j][0] +
            a[i][1] * a[j][1]
        );

    return G;
}

array::xxx
FEM::M(const std::array<Union::XY, 3>& elem, size_t area) const {
    double det = fabs(determinant(elem));
    double gammaKoef = materials[area].gamma * det / 24;
    std::array<std::array<double, 3>, 3> M;
    for (size_t i = 0; i < 3; i++)
    for (size_t j = 0; j < 3; j++) {
        M[i][j] =
            (i == j) ? (2 * gammaKoef) :
                       (    gammaKoef) ;
    }
    return  M;
}

void FEM::boundaryCondition() {
    using namespace ::Log;

    std::sort(
        boundarys.begin(),
        boundarys.end(),
        [](Union::Boundary& _left, Union::Boundary& _right){
            return _left.cond > _right.cond;
        }
    );

    for (size_t _count = 0; _count < _size.conds; _count++) {
        switch (boundarys[_count].cond)
        {
            case FIRST_BOUNDARY_COND:
                first(boundarys[_count]);
                break;
            case SECOND_BOUNDARY_COND:
                second(boundarys[_count]);
                break;
            case THIRD_BOUNDARY_COND:
                third(boundarys[_count]);
                break;
            default:
                Logger::append(getLog("There is no such condition"));
        }
    }
}

void FEM::first(const Union::Boundary& bound) {
    di[bound.nodeIdx[0]] = { 1 };
    di[bound.nodeIdx[1]] = { 1 };

    for (size_t i = 0; i < 2; i++)
        gb[bound.nodeIdx[i]] =
            Function::firstBound({
                nodes[bound.nodeIdx[i]].x,
                nodes[bound.nodeIdx[i]].y
            }, bound.type);

    for (size_t k = 0; k < 2; k++) {
        size_t node = bound.nodeIdx[k];
        for (size_t i = ig[node]; i < ig[node + 1]; i++) {
            if(di[jg[i]] != 1)
                gb[jg[i]] -= gg[i] * gb[node];
            gg[i] = 0;
        }

        for(size_t i = node + 1; i < _size.nodes; i++) {
            size_t lbeg = ig[i];
            size_t lend = ig[i + 1];
            for(size_t p = lbeg; p < lend; p++) {
                if(jg[p] == node) {
                    if(di[i] != 1)
                        gb[i] -= gg[p] * gb[node];
                    gg[p] = 0;
                }
            }
        }
    }
}

void FEM::second(const Union::Boundary& bound) {
    std::array<Union::XY, 2>
        coord_borders = {
            nodes[bound.nodeIdx[0]],
            nodes[bound.nodeIdx[1]]
        };

    double _koef =
        edgeLength(coord_borders) / 6;

    std::array<double, 2> corr_b;
    for (size_t i = 0; i < 2; i++)
        corr_b[i] = _koef * (
            2 * Function::secondBound({
                    nodes[bound.nodeIdx[i]].x,
                    nodes[bound.nodeIdx[i]].y
                }, bound.type) +
                Function::secondBound({
                    nodes[bound.nodeIdx[1 - i]].x,
                    nodes[bound.nodeIdx[1 - i]].y
                }, bound.type)
        );

    loc_b_to_global<2>(corr_b, bound);
}

void FEM::third(const Union::Boundary& bound) {
    std::array<Union::XY, 2> coord_borders = {
        nodes[bound.nodeIdx[0]],
        nodes[bound.nodeIdx[1]]
    };

    double _koef =
        materials[bound.area].betta *
        edgeLength(coord_borders) / 6;

    std::array<double, 2>                corr_b;
    std::array<std::array<double, 2>, 2> corr_a;

    for (size_t i = 0; i < 2; i++) {
        corr_b[i] = _koef * (
            2 * Function::thirdBound({
                    nodes[bound.nodeIdx[i]].x,
                    nodes[bound.nodeIdx[i]].y
                }, bound.type) +
                Function::thirdBound({
                    nodes[bound.nodeIdx[1 - i]].x,
                    nodes[bound.nodeIdx[1 - i]].y
                }, bound.type)
            );

        for (size_t j = 0; j < 2; j++) {
            corr_a[i][j] =
                (i == j) ? (2 * _koef) :
                           (    _koef) ;
        }
    }
    loc_b_to_global<2>(corr_b, bound);
    loc_A_to_global<2>(corr_a, bound);
}

bool FEM::readFile(const std::filesystem::path& path) {
    using namespace ::Log;
    bool isError { true };
    std::ifstream fin(path / "params.txt");
    isError &= is_open(fin, getLog("Error - params.txt"));
    fin >> _size.nodes
        >> _size.elems
        >> _size.areas
        >> _size.conds;
    fin.close();
    resize();
    fin.open(path / "nodes.txt");
    isError &= is_open(fin, getLog("Error - nodes.txt"));
    for (size_t i = 0; i < _size.nodes; i++)
        fin >> nodes[i].x >> nodes[i].y;
    fin.close();
    fin.open(path / "elems.txt");
    isError &= is_open(fin, getLog("Error - elems.txt"));
    for (size_t i = 0; i < _size.elems; i++) {
        fin >> elems[i].nodeIdx[0]
            >> elems[i].nodeIdx[1]
            >> elems[i].nodeIdx[2];
    }
    fin.close();
    fin.open(path / "areas.txt");
    isError &= is_open(fin, getLog("Error - areas.txt"));
    for (size_t i = 0; i < _size.areas; i++)
        fin >> materials[i].gamma
            >> materials[i].betta;

    for (size_t i = 0; i < _size.elems; i++)
        fin >> elems[i].area;
    fin.close();
    fin.open(path / "bords.txt");
    isError &= is_open(fin, getLog("Error - bords.txt"));
    for (size_t i = 0; i < _size.conds; i++)
        fin >> boundarys[i].area
            >> boundarys[i].nodeIdx[0]
            >> boundarys[i].nodeIdx[1]
            >> boundarys[i].cond
            >> boundarys[i].type;
    fin.close();
    return isError;
}
#endif /// _FEM_HPP_
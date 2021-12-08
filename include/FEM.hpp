//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~███████╗███████╗███╗░░░███╗~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~██╔════╝██╔════╝████╗░████║~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~█████╗░░█████╗░░██╔████╔██║~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~██╔══╝░░██╔══╝░░██║╚██╔╝██║~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~██║░░░░░███████╗██║░╚═╝░██║~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~╚═╝░░░░░╚══════╝╚═╝░░░░░╚═╝~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef _FEM_HPP_
#define _FEM_HPP_
#include "utils/lightweight.hpp"
#include "utils/overload.hpp"
#include "utils/friendly.hpp"
#include "Function.hpp"
#include "Logger.hpp"
#include "Union.hpp"

#include <algorithm>
#include <cmath>
#include <set>

#define FIRST_BOUNDARY_COND  1
#define SECOND_BOUNDARY_COND 2
#define THIRD_BOUNDARY_COND  3

class FEM
{
private:
    Union::Param _size;

    std::vector<Union::XY>       nodes;                                         /// Вектор узлов
    std::vector<Union::Element>  elems;                                         /// Вектор конечных элементов
    std::vector<Union::Boundary> boundarys;                                     /// Вектор краевых условий
    std::vector<Union::Material> materials;                                     /// Вектор материалов

    std::vector<double> gb;                                                     /// Глобальная матрица b
    std::vector<double> gg;
    std::vector<double> di;
    std::vector<size_t> ig;
    std::vector<size_t> jg;

public:
    FEM(std::filesystem::path _path) {
        assert(readFile(_path));                                                /// Читаем входные данные
        portrait(true);                                                         /// Создаём портрет
        global();                                                               /// Создание глобальной матрицы
        boundaryCondition();                                                    /// Учёт краевых условий
    }
    ~FEM() { }

    void printAll()    const;
    void printSparse() const;

    void writeFile(
        const std::filesystem::path&,
        const double,
        const size_t
    ) const;

    Friendly* takeDate();
    size_t getNodes() { return _size.nodes; }

private:
    void global();                                                              /// Функция построения глобальной матрицы и вектора

    template<size_t N, typename _Struct>                                        /// Шаблон занесения локальной
    void loc_A_to_global(                                                       /// матрицы в глобальную
        const std::array<std::array<double, N>, N>&,
        const _Struct&
    );

    template<size_t N, typename _Struct>                                        /// Шаблон занемения локального
    void loc_b_to_global(const std::array<double, N>&, const _Struct& );        /// вектора в глобыльный

    array::xxx localA(const std::array<Union::XY, 3>&, size_t) const;
    array::x   buildF(const std::array<Union::XY, 3>&, size_t) const;

    array::xxx G(const std::array<Union::XY, 3>&, size_t) const;                /// Построение матрицы жесткости
    array::xxx M(const std::array<Union::XY, 3>&, size_t) const;                /// Построение матрицы масс

    bool readFile(const std::filesystem::path&  );
    void portrait(const bool isWriteList = false);

    void boundaryCondition();
    void first (const Union::Boundary& bound);                                  /// Первое краевое условие
    void second(const Union::Boundary& bound);                                  /// Второе краевое условие
    void third (const Union::Boundary& bound);                                  /// Третье краевое условие

    void resize();
};

Friendly* FEM::takeDate() {
    Friendly* _friend =
        new Friendly {
            gb,
            gg,
            di,
            ig,
            jg
        };
    return _friend;
}

void FEM::global() {

    std::array<Union::XY, 3> coords;

    for (size_t i = 0; i < _size.elems; i++) {
        for (size_t j = 0; j < 3; j++) {
            size_t point = elems[i].nodeIdx[j];
            coords[j].x = nodes[point].x;
            coords[j].y = nodes[point].y;
        }
        array::x   local_b = buildF(coords, elems[i].area);
        array::xxx local_A = localA(coords, elems[i].area);

        #if DEBUG != 0
        pretty(local_A);
        #endif

        loc_A_to_global<3>(local_A, elems[i]);
        loc_b_to_global<3>(local_b, elems[i]);
    }
}

void FEM::boundaryCondition() {
    using namespace ::Log;

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
    di[bound.nodeIdx[0]] = { 1 };                                               /// Ставим всесто диагональных
    di[bound.nodeIdx[1]] = { 1 };                                               /// элементов единицу

    for (size_t i = 0; i < 2; i++)                                              /// В вектор правой части записываем
        gb[bound.nodeIdx[i]] =                                                  /// значение краевого условия
            Function::firstBound({
                nodes[bound.nodeIdx[i]].x,
                nodes[bound.nodeIdx[i]].y
            }, bound.type);

    for (size_t k = 0; k < 2; k++) {                                            /// Зануляем в строке все стоящие элементы
        size_t node = bound.nodeIdx[k];                                         /// кроме диагонального и делаем симметричной
        for (size_t i = ig[node]; i < ig[node + 1]; i++) {
            gb[jg[i]] -= gg[i] * gb[node];                                      /// Отнимаем от правой части зануляемый элемент
            gg[i] = 0;                                                          /// Зануление в нижнем треугольнике
        }

        for(size_t i = node + 1; i < _size.nodes; i++) {                        /// Зануление в верхнем треугольнике
            size_t lbeg = ig[i];
            size_t lend = ig[i + 1];
            for(size_t p = lbeg; p < lend; p++) {
                if(jg[p] == node) {
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

    double _koef = edgeLength(coord_borders) / 6;                               /// Коэффициент для корректирующего вектора

    std::array<double, 2> corr_b;                                               /// Корректирующий вектор
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

    std::array<std::array<double, 2>, 2> corr_a;

    std::array<double, 2> corr_b;                                               /// Корректирующий вектор
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
template<size_t N, typename _Struct>
void FEM::loc_A_to_global(
        const std::array<std::array<double, N>, N>& locA,
        const _Struct& elem) {

    using            ::std::vector;
    using iterator = ::std::vector<size_t>::iterator;

    for (size_t i = 0; i < N; i++) {
        di[elem.nodeIdx[i]] += locA[i][i];

        for (int j = 0; j < i; j++) {
            size_t a = elem.nodeIdx[i];
            size_t b = elem.nodeIdx[j];
            if (a < b) std::swap(a, b);

            if (ig[a + 1] > ig[a]) {
                iterator _beg = jg.begin() + ig[a];
                iterator _end = jg.begin() + ig[a + 1] - ig[0];

                auto _itr = std::lower_bound(_beg, _end, b);
                auto _idx = _itr - jg.begin();
                gg[_idx] += locA[i][j];
            }
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

array::x FEM::buildF(const std::array<Union::XY, 3>& elem, size_t area) const {
    std::array<double, 3> function {
        Function::f(elem[0], area),
        Function::f(elem[1], area),
        Function::f(elem[2], area)
    };

    double det_D = abs(determinant(elem)) / 24;
    return {                                                                    /// Локальный вектор b
        det_D * (2 * function[0] + function[1] + function[2]),
        det_D * (2 * function[1] + function[0] + function[2]),
        det_D * (2 * function[2] + function[0] + function[1]),
    };
}

array::xxx FEM::localA(const std::array<Union::XY, 3>& elem, size_t area) const {
    std::array<std::array<double, 3>, 3> G = FEM::G(elem, area);
    std::array<std::array<double, 3>, 3> M = FEM::M(elem, area);
    std::array<std::array<double, 3>, 3> A = G + M;                             /// Локальная матрица A
    return A;

}

array::xxx FEM::G(const std::array<Union::XY, 3>& elem, size_t area) const {
    double det   = abs(determinant(elem));
    double _koef = Function::lambda(area) / (2 * det);

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
        G[i][j] = _koef * (a[i][0] * a[j][0] + a[i][1] * a[j][1]);

    return G;
}

array::xxx FEM::M(const std::array<Union::XY, 3>& elem, size_t area) const {
    double det = abs(determinant(elem));
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

    jg.resize(ig[N] - ig[0]);                                                   /// Выделение памяти происходит в данном методе, т.к. размер
    gg.resize(ig[N] - ig[0]);                                                   /// векторов jd и gg равен последнему элементу вектора ig

   for (size_t index = 0, i = 1; i < list.size(); i++)
    for (size_t value : list[i])
        jg[index++] = value;

    #if DEBUG != 0
    if (isWriteList) {                                                          /// Вывод списка связности если isWriteList = true
        std::cout << "list: " << '\n';
        for (size_t i = 0; i < list.size(); i++) {
            std::cout << i << ':' << ' ';
            for (size_t j : list[i])
                std::cout << j << ' ';
            std::cout << std::endl;
        }
    }
    #endif
}

void FEM::printAll() const {
    #define PRINTLINE \
        for (size_t i = 0; i < 20; std::cout << '-', i++);
    #define ENDLINE std::cout << '\n';
    SetConsoleOutputCP(65001);
    PRINTLINE ENDLINE
    std::cout << "PARAMS:         "                 << '\n';
    std::cout << "Size nodes:     " << _size.nodes  << '\n';
    std::cout << "Size element:   " << _size.elems  << '\n';
    std::cout << "Size areas:     " << _size.areas  << '\n';
    std::cout << "Size condition: " << _size.conds  << '\n';
    PRINTLINE ENDLINE
    std::cout << std::setw(4) << "X" << std::setw(4) << "Y" << '\n';
    for (size_t i = 0; i < _size.nodes; i++)
        std::cout << std::setw(4) << nodes[i].x
                  << std::setw(4) << nodes[i].y << '\n';
    PRINTLINE ENDLINE
    std::cout << "Elements: " << '\n';
    for (size_t i = 0; i < _size.elems; i++)
        std::cout << elems[i].nodeIdx[0] << ' '
                  << elems[i].nodeIdx[1] << ' '
                  << elems[i].nodeIdx[2] << " -> area "
                  << elems[i].area       << '\n';
    PRINTLINE ENDLINE
    std::cout << "Areas: " << '\n';
    for (size_t i = 0; i < _size.areas; i++) {
        std::cout << "\u03B3 = " << materials[i].gamma  << ',' << ' '
                  << "\u03B2 = " << materials[i].betta
                  << " -> area " << i << '\n';
    }
    PRINTLINE ENDLINE
    std::cout << "Borders: " << '\n';
    for (size_t i = 0; i < _size.conds; i++)
        std::cout << boundarys[i].area       << ' '
                  << boundarys[i].nodeIdx[0] << ' '
                  << boundarys[i].nodeIdx[1] << ' '
                  << boundarys[i].cond       << ' '
                  << boundarys[i].type       << ' ' << '\n';
    PRINTLINE ENDLINE
    #undef PRINTLINE
    #undef ENDLINE
}

void FEM::printSparse() const {
    #define PRINTLINE \
        for (size_t i = 0; i < 20; std::cout << '-', i++); \
        std::cout << '\n';
    PRINTLINE
    std::cout << "ig: "; print(ig);
    std::cout << "jg: "; print(jg);
    std::cout << "di: "; print(di);
    std::cout << "gg: "; print(gg);
    PRINTLINE
    #undef PRINTLINE
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

    resize();                                                                   /// Выделение памяти под вектора
    std::fill_n(ig.begin(), 2, 0);                                              /// Заполнение первых 2 ячеек веткора ig нулями
                                                                                ///         (при желании можно поменять на единицы)
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

    std::sort(                                                                  /// Сортировка по роду краевого условия
        boundarys.begin(),                                                      /// Первое краевое условие,
        boundarys.end(),                                                        /// должно быть последним
        [](Union::Boundary& _left, Union::Boundary& _right){
            return _left.cond > _right.cond;
        }
    );

    return isError;
}

void FEM::writeFile(
        const std::filesystem::path& _path,
        const double _eps,
        const size_t _max_iter) const {

    std::filesystem::create_directories(_path);
    bool is_dir = std::filesystem::is_directory(_path);

    using namespace ::Log;
    if (not is_dir) assert(
        Logger::append(getLog("Error - create directory"))
    );

    std::ofstream fout(_path / "kuslau.txt");
    fout << _size.nodes             << '\n';
    fout << std::scientific << _eps << '\n';
    fout << _max_iter;
    fout.close();

    Output::write(_path / "gg.txt", gg, { 14, ' ' });
    Output::write(_path / "di.txt", di, { 14, ' ' });
    Output::write(_path / "jg.txt", jg);
    Output::write(_path / "ig.txt", ig);
    Output::write(_path / "gb.txt", gb);
}

void FEM::resize() {
    nodes.    resize(  _size.nodes  );                                          /// Выделение памяти для координат узлов
    elems.    resize(  _size.elems  );                                          /// Выделение памяти для хранение количества елементов
    boundarys.resize(  _size.conds  );                                          /// Выделение памяти для хранения краевых условий
    materials.resize(  _size.areas  );                                          /// Выделение памяти для хранения материалов

    gb.resize(  _size.nodes  );                                                 /// Выделение памяти для глобального вектора
    di.resize(  _size.nodes  );
    ig.resize(_size.nodes + 1);
}

#undef FIRST_BOUNDARY_COND
#undef SECOND_BOUNDARY_COND
#undef THIRD_BOUNDARY_COND
#endif /// _FEM_HPP_
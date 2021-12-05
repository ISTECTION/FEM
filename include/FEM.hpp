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
    std::vector<Union::Boundary> boundarys;                                       /// Вектор краевых условий
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
        global_Matrix();                                                        /// Создание глобальной матрицы
        boundaryСondition();                                                    /// Учёт краевых условий

    }
    ~FEM() { }

    void printAll()    const;
    void printSparse() const;

    void writeFile(
        const std::filesystem::path&,
        const double,
        const size_t
    ) const;

private:
    void global_Matrix();
    void toGlobal(const array::x&, const array::xxx&, const Union::Element);    /// Функция занесения локальной матрицы в глобальную

    array::xxx localA(const std::array<Union::XY, 3>&, size_t) const;
    array::x   buildF(const std::array<Union::XY, 3>&, size_t) const;

    array::xxx G(const std::array<Union::XY, 3>&, size_t) const;                /// Построение матрицы жесткости
    array::xxx M(const std::array<Union::XY, 3>&, size_t) const;                /// Построение матрицы масс

    bool readFile(const std::filesystem::path&  );
    void portrait(const bool isWriteList = false);

    void boundaryСondition();
    void first (const Union::Boundary& bound);                                  /// Первое краевое условие
    void second(const Union::Boundary& bound);                                  /// Второе краевое условие
    void third (const Union::Boundary& bound);                                  /// Третье краевое условие

    void resize();
};

void FEM::global_Matrix() {

    std::array<Union::XY, 3> coords;

    for (size_t i = 0; i < _size.elems; i++) {
        for (size_t j = 0; j < 3; j++) {
            size_t point = elems[i].nodeIdx[j];
            coords[j].x = nodes[point].x;
            coords[j].y = nodes[point].y;
        }
        array::x   local_b = buildF(coords, elems[i].area);
        array::xxx local_A = localA(coords, elems[i].area);
        pretty(local_A);
        std::cout << '\n';
        for(size_t i = 0; i < 3; i++)
            std::cout << local_b[i] << ' ';
        std::cout << '\n';


        toGlobal(local_b, local_A, elems[i]);
    }
}

void FEM::boundaryСondition() {
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
    di[bound.bordIdx[0]] = { 1 };                                               /// Ставим всесто диагональных
    di[bound.bordIdx[1]] = { 1 };                                               /// элементов единицу

    gb[bound.bordIdx[0]] =                                                      /// В вектор правой части
        Function::firstBound(                                                   /// записываем значение
            {                                                                   /// краевого условия
                nodes[bound.bordIdx[0]].x,
                nodes[bound.bordIdx[0]].y
            },
            bound.type
    );

    gb[bound.bordIdx[0]] =
        Function::firstBound(
            {
                nodes[bound.bordIdx[1]].x,
                nodes[bound.bordIdx[1]].y
            },
            bound.type
    );
}

void FEM::second(const Union::Boundary& bound) {
}

void FEM::third(const Union::Boundary& bound) {
}

void FEM::toGlobal(
        const array::x&   local_b,
        const array::xxx& local_A,
        const Union::Element elem) {

    using            ::std::vector;
    using iterator = ::std::vector<size_t>::iterator;

    for (size_t i = 0; i < 3; i++) {
        di[elem.nodeIdx[i]] += local_A[i][i];
        gb[elem.nodeIdx[i]] += local_b[i];

        for (int j = 0; j < i; j++) {
            size_t a = elem.nodeIdx[i];
            size_t b = elem.nodeIdx[j];
            if (a < b) std::swap(a, b);

            if (ig[a + 1] > ig[a]) {
                iterator _beg = jg.begin() + ig[a];
                iterator _end = jg.begin() + ig[a + 1] - ig[0];

                auto _itr = std::lower_bound(_beg, _end, b);
                auto _idx = _itr - jg.begin();
                gg[_idx] += local_A[i][j];
            }
        }
    }
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

    if (isWriteList) {                                                          /// Вывод списка связности если isWriteList = true
        std::cout << "list: " << '\n';
        for (size_t i = 0; i < list.size(); i++) {
            std::cout << i << ':' << ' ';
            for (size_t j : list[i])
                std::cout << j << ' ';
            std::cout << std::endl;
        }
    }
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
                  << boundarys[i].bordIdx[0] << ' '
                  << boundarys[i].bordIdx[1] << ' '
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
            >> boundarys[i].bordIdx[0]
            >> boundarys[i].bordIdx[1]
            >> boundarys[i].cond
            >> boundarys[i].type;
    fin.close();
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

    Output::write(_path / "gg.txt", gg, { ' ', 14 });
    Output::write(_path / "di.txt", di, { ' ', 14 });
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
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
#include "LOS/LOS.hpp"
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
    Union::Param _size;                                                         /// Параметры задачи

    std::vector<Union::XY>       nodes;                                         /// Вектор узлов
    std::vector<Union::Element>  elems;                                         /// Вектор конечных элементов
    std::vector<Union::Boundary> boundarys;                                     /// Вектор краевых условий
    std::vector<Union::Material> materials;                                     /// Вектор материалов

    std::vector<double> gb;                                                     /// Глобальная матрица b
    std::vector<double> gg;                                                     /// Значения нижнего треугольника
    std::vector<double> di;                                                     /// Диагональные значения
    std::vector<size_t> ig;                                                     /// Указатели начала строк
    std::vector<size_t> jg;                                                     /// Указатели столбцов значений

    std::vector<double> _z;                                                     /// Вектор решений
public:
    FEM(std::filesystem::path _path) {
        assert(readFile(_path));                                                /// Читаем входные данные
        portrait(true);                                                         /// Создаём портрет
        global();                                                               /// Создание глобальной матрицы
        boundaryCondition();                                                    /// Учёт краевых условий
    }

    void startLOS(const std::filesystem::path& _out) {
        using ::Cond::HOLLESKY;                                                 /// Неполное ращложение Холецкого
        using ::Cond::DIAGONAL;                                                 /// Диагональная предобусловленность
        using ::Cond::NONE;                                                     /// Без предобусловленности

        const double _eps = 1E-20;
        const double _itr = 10000;

        #if DEBUG != 0
            writeFile(_out, _eps, _itr);
            LOS<double> _LOS(_out);
        #else
            LOS<double> _LOS (
                getDate(),
                getNodes(),
                _eps, _itr);
        #endif

        _LOS.solve(DIAGONAL, false);
        _z = std::move(_LOS.getX());
    }

    void printZ()         const;                                                /// Функция печатает вектор решений
    void printAll()       const;                                                /// Распечатать все вхожные данные
    void printSparse()    const;                                                /// Распечатать в разреженном формате
    void printAnalitics();                                                      /// Функция вычисления аналитического решения
    void createTable();

    void writeFile(                                                             /// Запись файлов
        const std::filesystem::path&,                                           /// 1. Путь
        const double,                                                           /// 2. Epsilon для вычислений LOS
        const size_t                                                            /// 3. Максимальное количество итераций
    ) const;

    size_t    getNodes() { return _size.nodes; }                                /// Получить количество узлов
    Friendly* getDate();                                                        /// Получить указатели векторов
    Sparse    getSparse();                                                      /// Получить матрицу в разреженном формате

    std::vector<double> getAnalitics();
    double getValue   (double, double) const;                                   /// Получить значение в любой точке
    void   operator() (double, double);                                         /// Перегрузка получения значения в любой точке
private:
    void global();                                                              /// Функция построения глобальных данных
    void resize();                                                              /// Функция выделения памяти

    array::x   localB(const std::array<Union::XY, 3>&, size_t) const;           /// Построение локального вектора
    array::xxx localA(const std::array<Union::XY, 3>&, size_t) const;           /// Вычисление локальной матрицы

    array::xxx G(const std::array<Union::XY, 3>&, size_t) const;                /// Вычисление матрицы жесткости
    array::xxx M(const std::array<Union::XY, 3>&, size_t) const;                /// Вычисление матрицы масс

    bool readFile(const std::filesystem::path&  );                              /// Считывание файлов
    void portrait(const bool isWriteList = false);                              /// Создание портрета матрицы

    void boundaryCondition();                                                   /// Головная подпрограмма краевых условий
    void first (const Union::Boundary& bound);                                  /// Первое краевое условие
    void second(const Union::Boundary& bound);                                  /// Второе краевое условие
    void third (const Union::Boundary& bound);                                  /// Третье краевое условие

    template<size_t N,                                                          /// Шаблон занесения локальной
        typename _Struct>                                                       /// матрицы в глобальную
        void loc_A_to_global(
            const std::array<std::array<double, N>, N>&,                        /// Массив значений  (3x3 & 2x2)
            const _Struct&                                                      /// Структура данных (B)
        );

    template<size_t N,                                                          /// Шаблон занесения локального
        typename _Struct>                                                       /// вектора в глобальный
        void loc_b_to_global(
            const std::array<double, N>&,
            const _Struct&
        );
};

void FEM::portrait(const bool isWriteList) {

    const size_t N {    _size.nodes     };
    std::vector<std::set<size_t>> list(N);

    for (size_t el = 0; el < _size.elems; el++)
    for (size_t point = 0; point < 3; point++) {                                /// Можно изменить на point < 2
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

    jg.resize(ig[N]);                                                           /// Выделение памяти происходит в данном методе, т.к. размер
    gg.resize(ig[N]);                                                           /// векторов jd и gg равен последнему элементу вектора ig

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

void FEM::global() {
    std::array<Union::XY, 3> coords;                                            /// Массив координат элемента
    for (size_t i = 0; i < _size.elems; i++) {
        for (size_t j = 0; j < 3; j++) {
            size_t point = elems[i].nodeIdx[j];                                 /// point - номер узла
            coords[j].x = nodes[point].x;                                       /// Координата узла по X
            coords[j].y = nodes[point].y;                                       /// Координата узла по Y
        }
        array::x   local_b = localB(coords, elems[i].area);                     /// Вычисление локального вектора
        array::xxx local_A = localA(coords, elems[i].area);                     /// Вычисления локальной матрицы

        #if DEBUG != 0
        std::cout << "Element: "                                                /// Вывод на экран конечного элемента
                    << elems[i].nodeIdx[0] << ' '                               /// который мы будем добавлять
                    << elems[i].nodeIdx[1] << ' '                               /// в глобальную матрицу
                    << elems[i].nodeIdx[2] << '\n';
        pretty(local_b);                                                        /// Вывод на экран локального вектора
        pretty(local_A);                                                        /// Вывод на экран локальной матрицы
        #endif

        loc_A_to_global<3>(local_A, elems[i]);                                  /// Занесение локальной матрицы в глобальную
        loc_b_to_global<3>(local_b, elems[i]);                                  /// Занесение локального вектора в глобальный

        #if DEBUG != 0
        prettyG(getSparse());                                                   /// Вывод на экран глобальной матрицы
        pretty(gb);                                                             /// Вывод на экран глобального вектора
        #endif
    }
}

template<size_t N, typename _Struct>
void FEM::loc_A_to_global(
        const std::array<std::array<double, N>, N>& locA,
        const _Struct& elem) {

    using            ::std::vector;
    using iterator = ::std::vector<size_t>::iterator;

    for (size_t i = 0; i < N; i++) {
        di[elem.nodeIdx[i]] += locA[i][i];                                      /// Занесение диагональных елементов

        for (size_t j = 0; j < i; j++) {
            size_t a = elem.nodeIdx[i];                                         /// max
            size_t b = elem.nodeIdx[j];                                         /// min
            if (a < b) std::swap(a, b);                                         /// swap

            if (ig[a + 1] > ig[a]) {
                iterator _beg = jg.begin() + ig[a];                             /// Указатель начала строки
                iterator _end = jg.begin() + ig[a + 1];                         /// Указатель конца строки

                auto _itr = std::lower_bound(_beg, _end, b);                    /// Индекс + jg.begin() елемента b
                auto _idx = _itr - jg.begin();                                  /// Индекс элемента b из вектора jg
                gg[_idx] += locA[i][j];                                         /// Занесения локальной матрицы
            }
        }
    }
}

template<size_t N, typename _Struct>
void FEM::loc_b_to_global(
        const std::array<double, N>& loc_b,
        const _Struct& elem) {

    for (size_t i = 0; i < N; i++)                                              /// Занесение локального вектора
        gb[elem.nodeIdx[i]] += loc_b[i];
}

array::x
FEM::localB(const std::array<Union::XY, 3>& elem, size_t area) const {
    std::array<double, 3> function {
        Function::f(elem[0], area),
        Function::f(elem[1], area),
        Function::f(elem[2], area)
    };

    double det_D = fabs(determinant(elem)) / 24;
    return {                                                                    /// Возвращаем вычисленный локальный вектор
        det_D * (2 * function[0] + function[1] + function[2]),
        det_D * (2 * function[1] + function[0] + function[2]),
        det_D * (2 * function[2] + function[0] + function[1])
    };
}

array::xxx
FEM::localA(const std::array<Union::XY, 3>& elem, size_t area) const {
    std::array<std::array<double, 3>, 3> G = FEM::G(elem, area);
    std::array<std::array<double, 3>, 3> M = FEM::M(elem, area);
    std::array<std::array<double, 3>, 3> A = G + M;                             /// Локальная матрица A
    return A;
}

array::xxx
FEM::G(const std::array<Union::XY, 3>& elem, size_t area) const {
    double det   = fabs(determinant(elem));
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

    for (size_t _count = 0; _count < _size.conds; _count++) {
        switch (boundarys[_count].cond)
        {
            case FIRST_BOUNDARY_COND:                                           /// Кейс первого краевого условия
                first(boundarys[_count]);
                break;
            case SECOND_BOUNDARY_COND:                                          /// Кейс второго краевого условия
                second(boundarys[_count]);
                break;
            case THIRD_BOUNDARY_COND:                                           /// Кейс третьего краевого условия
                third(boundarys[_count]);
                break;
            default:
                Logger::append(getLog("There is no such condition"));
        }
    }
}

void FEM::first(const Union::Boundary& bound) {
    di[bound.nodeIdx[0]] = { 1 };                                               /// Ставим вместо диагональных
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
            if(di[jg[i]] != 1)
                gb[jg[i]] -= gg[i] * gb[node];                                  /// Отнимаем от правой части зануляемый элемент
            gg[i] = 0;                                                          /// Зануление в нижнем треугольнике
        }

        for(size_t i = node + 1; i < _size.nodes; i++) {                        /// Зануление в верхнем треугольнике
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
        edgeLength(coord_borders) / 6;                                          /// Коэффициент для корректирующего вектора

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

    std::array<double, 2>                corr_b;                                /// Корректирующий вектор
    std::array<std::array<double, 2>, 2> corr_a;                                /// Корректирующая матрица

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
    resize();                                                                   /// Выделение памяти под вектора
    std::fill_n(ig.begin(), 2, 0);                                              /// Заполнение первых 2 ячеек веткора ig нулями
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

void FEM::printAll() const {
    #define PRINTLINE \
        for (size_t i = 0; i < 58; std::cout << '-', i++);
    #define ENDLINE std::cout << '\n';
    SetConsoleOutputCP(65001);
    PRINTLINE ENDLINE
    std::cout << "PARAMS:         "                 << '\n';
    std::cout << "Size nodes:     " << _size.nodes  << '\n';
    std::cout << "Size element:   " << _size.elems  << '\n';
    std::cout << "Size areas:     " << _size.areas  << '\n';
    std::cout << "Size condition: " << _size.conds  << '\n';
    PRINTLINE ENDLINE
    std::cout << std::setw(8) << "X" << std::setw(8) << "Y" << '\n';
    for (size_t i = 0; i < _size.nodes; i++)
        std::cout << std::setw(8) << nodes[i].x
                  << std::setw(8) << nodes[i].y << '\n';
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
        for (size_t i = 0; i < 58; std::cout << '-', i++); \
        std::cout << '\n';
    PRINTLINE
    std::cout << "ig: "; print(ig);
    std::cout << "jg: "; print(jg);
    std::cout << "di: "; print(di);
    std::cout << "gg: "; print(gg);
    PRINTLINE
    #undef PRINTLINE
}

void FEM::writeFile(
        const std::filesystem::path& _path,
        const double _eps,
        const size_t _max_iter) const { {

    std::filesystem::create_directories(_path);
    bool is_dir = std::filesystem::is_directory(_path);

    using namespace ::Log;
    if (not is_dir) assert (
        Logger::append(getLog("Error - create directory"))
    ); }

    std::ofstream fout(_path / "kuslau.txt");
    fout << _size.nodes             << '\n';
    fout << std::scientific << _eps << '\n';
    fout << _max_iter;
    fout.close();

    Output::write(_path / "gg.txt", gg, { 14, ' ' });
    Output::write(_path / "di.txt", di, { 14, ' ' });
    Output::write(_path / "gb.txt", gb, { 14, ' ' });
    Output::write(_path / "jg.txt", jg);
    Output::write(_path / "ig.txt", ig);
}

void FEM::resize() {
    nodes.    resize(_size.nodes);
    elems.    resize(_size.elems);
    boundarys.resize(_size.conds);
    materials.resize(_size.areas);

    gb.resize(  _size.nodes  );
    di.resize(  _size.nodes  );
    ig.resize(_size.nodes + 1);
}

double FEM::getValue(double x, double y) const {
    #define POINT_OUTSIDE_AREA 3

    std::array<Union::XY, 3> _coord;                                            /// Массив координат элемента
    for (size_t _elem = 0; _elem < _size.nodes; _elem++) {

        Union::Element elem = elems[_elem];
         for (size_t j = 0; j < 3; j++) {
            size_t point = elems[_elem].nodeIdx[j];                             /// point - номер узла
            _coord[j].x = nodes[point].x;                                       /// Координата узла по X
            _coord[j].y = nodes[point].y;                                       /// Координата узла по Y
        }

        if (Relativ_METHOD::inTriangle(x, y, _coord)) {
            struct Barycentric {
                double m1;
                double m2;
                double m3;
            };

            struct Barycentric _Bar_mas;
            _Bar_mas.m1 = ((          y - _coord[0].y) * (_coord[1].x - _coord[0].x)  -
                           (          x - _coord[0].x) * (_coord[1].y - _coord[0].y)) /
                          ((_coord[2].y - _coord[0].y) * (_coord[1].x - _coord[0].x)  -
                           (_coord[2].x - _coord[0].x) * (_coord[1].y - _coord[0].y)) ;

            _Bar_mas.m2 = ((          y - _coord[0].y) * (_coord[2].x - _coord[0].x)  -
                           (          x - _coord[0].x) * (_coord[2].y - _coord[0].y)) /
                          ((_coord[1].y - _coord[0].y) * (_coord[2].x - _coord[0].x)  -
                           (_coord[1].x - _coord[0].x) * (_coord[2].y - _coord[0].y)) ;

            _Bar_mas.m3 = ((          y - _coord[2].y) * (_coord[1].x - _coord[2].x)  -
                           (          x - _coord[2].x) * (_coord[1].y - _coord[2].y)) /
                          ((_coord[0].y - _coord[2].y) * (_coord[1].x - _coord[2].x)  -
                           (_coord[0].x - _coord[2].x) * (_coord[1].y - _coord[2].y)) ;

            return {
                _Bar_mas.m1 * _z[elem.nodeIdx[2]] +
                _Bar_mas.m2 * _z[elem.nodeIdx[1]] +
                _Bar_mas.m3 * _z[elem.nodeIdx[0]]
            };
        }
    }
    Logger::append(getLog("coord (" + std::to_string(x)
        + ", " + std::to_string(y) + ") not in area"));

    std::exit(POINT_OUTSIDE_AREA);
    #undef    POINT_OUTSIDE_AREA
}

Friendly* FEM::getDate() {
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

Sparse FEM::getSparse() {
    return Sparse {
            gg,
            di,
            ig,
            jg
    };
}

std::vector<double> FEM::getAnalitics() {
    std::vector<double> ax;
    ax.resize(_size.nodes);

    for (const auto& _elem : elems) {
        for (size_t i = 0; i < 3; i++)
            ax[_elem.nodeIdx[i]] =
                Function::analitics(
                    nodes[_elem.nodeIdx[i]],
                    _elem.area
                );
    }
    return ax;
}

void FEM::printAnalitics() {
    std::cout << "ANALITIC: ";
    print(getAnalitics(), 14);
}

void FEM::operator() (double x, double y) {
    const std::streamsize CN = 5; /// Количество выводимых знаков числа

    double z = getValue(x, y);

    std::streamsize p = std::cout.precision();
    std::cout.precision(3);
    std::cout.setf(std::ios::fixed | std::ios::left);
    std::cout << std::setw(8 + CN) << "x: " + std::to_string(x).substr(0, CN)
              << std::setw(8 + CN) << "y: " + std::to_string(y).substr(0, CN)
              << std::setw(8 + CN) << "u: " + std::to_string(z).substr(0, CN)
              << '\n';
    std::cout.unsetf(std::ios::fixed);
    std::cout.precision(p);
}

void FEM::createTable() {
    #define PRINTLINE for(size_t i = 0; i < 58; i++) \
                                    _ostream << '-';
    #define ENDLINE _ostream << '\n';

    std::vector<double> _ax = getAnalitics();
    std::ostringstream _ostream;
    PRINTLINE ENDLINE
    _ostream
        << std::setw(22) << std::fixed      << 'x'
        << std::setw(22) << std::fixed      << "x*"
        << std::setw(14) << std::scientific << "x* - x";
    ENDLINE
    PRINTLINE ENDLINE
    for (size_t i = 0; i < _ax.size(); i++) {
        _ostream.precision(14);
        _ostream
            << std::setw(22) << std::fixed << _ax[i]
            << std::setw(22) << std::fixed << _z [i];

        _ostream.precision(2);
        _ostream
            << std::setw(14) << std::scientific << _z[i] - _ax[i];
        ENDLINE
    }
    PRINTLINE ENDLINE
    std::cout << _ostream.str();
    #undef PRINTLINE
    #undef ENDLINE
}

void FEM::printZ() const { print(_z, 14); }
#undef FIRST_BOUNDARY_COND
#undef SECOND_BOUNDARY_COND
#undef THIRD_BOUNDARY_COND
#endif /// _FEM_HPP_
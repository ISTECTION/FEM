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

#include <filesystem>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <set>

class FEM
{
private:
    Union::Param _size;

    std::vector<Union::XY>       nodes;                                         /// Вектор узлов
    std::vector<Union::Element>  elems;                                         /// Вектор конечных элементов
    std::vector<Union::Border>   borders;                                       /// Вектор краевых условий
    std::vector<Union::Material> materials;                                     /// Структура материалов

    std::vector<double> gg;
    std::vector<double> di;
    std::vector<size_t> ig;
    std::vector<size_t> jg;

    std::vector<std::vector<double>> global_A;                                  /// Глобальаня матрица A
    std::vector            <double>  global_b;                                  /// Глобальная матрица b

public:
    FEM(std::filesystem::path _path) {

        assert(readFile(_path));                                                /// Читаем входные данные
        portrait(true);                                                         /// Создаём портрет
        global_Matrix();
    }
    ~FEM() { }

    void printAll()    const;
    void printSparse() const;

    void writeFile(const std::filesystem::path& ) const;

private:
    void global_Matrix();
    void local(const std::array<Union::XY, 3>& xyElem, size_t area);

    array::xxx G(const std::array<Union::XY, 3>& xyElem, size_t area);          /// Построение матрицы жесткости
    array::xxx M(const std::array<Union::XY, 3>& xyElem, size_t area);          /// Построение матрицы масс

    bool readFile(const std::filesystem::path& );
    void portrait(const bool isWriteList = false);


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
        local(coords, elems[i].area);
    }
}

void FEM::local(const std::array<Union::XY, 3>& xyElem, size_t area) {

    std::array<double, 3> function {
        Function::f(xyElem[0], area),
        Function::f(xyElem[1], area),
        Function::f(xyElem[2], area)
    };

    double det_D = abs(determinant(xyElem)) / 24;
    std::array<double, 3> local_b {                                             /// Локальный вектор b
        det_D * (2 * function[0] + function[1] + function[2]),
        det_D * (2 * function[1] + function[0] + function[2]),
        det_D * (2 * function[2] + function[0] + function[1]),
    };


    std::array<std::array<double, 3>, 3> G = FEM::G(xyElem, area);
    std::array<std::array<double, 3>, 3> M = FEM::M(xyElem, area);
    std::array<std::array<double, 3>, 3> local_A = G + M;                       /// Локальная матрица A

}

array::xxx FEM::G(const std::array<Union::XY, 3>& xyElem, size_t area) {
    double det = abs(determinant(xyElem));


}

array::xxx FEM::M(const std::array<Union::XY, 3>& xyElem, size_t area) {
    double det = abs(determinant(xyElem));
    double gammaKoef = Function::gammaKoef(area) * det / 24;
    std::array<std::array<double, 3>, 3> M;

    for (size_t i = 0; i < 3; i++)
    for (size_t j = 0; j < 3; j++) {
        M[i][j] = {
            i == j ? 2 * gammaKoef : gammaKoef
        };
    }

    for(size_t i = 0; i < 3; i++) {
        for(size_t j = 0; j < 3; j++)
            std::cout << M[i][j] << " ";
        std::cout << std::endl;
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

    if (isWriteList) {
        std::cout << "list: " << '\n';
        for (size_t i = 0; i < list.size(); i++) {
            std::cout << i << ':' << ' ';
            for (size_t j : list[i])
                std::cout << j << ' ';
            std::cout << std::endl;
        }
    }

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ///
    // std::cout << std::endl;
    // std::cout << "ig: " << '\n';
    // for (size_t i = 0; i < ig.size(); i++)
    //     std::cout << ig[i] << ' ';
    // std::cout << std::endl;
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ///
    // std::cout << std::endl;
    // std::cout << "jg: " << '\n';
    // for (size_t i = 0; i < jg.size(); i++)
    //     std::cout << jg[i] << ' ';
    // std::cout << std::endl;
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ///
}

void FEM::printAll() const {
    #define PRINTLINE \
        for (size_t i = 0; i < 20; std::cout << '-', i++);
    #define ENDLINE std::cout << '\n';
    system("chcp 65001");
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
        std::cout << "\u03BB = " << materials[i].lambda << ',' << ' '
                  << "\u03B3 = " << materials[i].gamma  << ',' << ' '
                  << "\u03B2 = " << materials[i].betta
                  << " -> area " << i << '\n';
    }
    PRINTLINE ENDLINE
    std::cout << "Borders: " << '\n';
    for (size_t i = 0; i < _size.conds; i++)
        std::cout << borders[i].area       << ' '
                  << borders[i].bordIdx[0] << ' '
                  << borders[i].bordIdx[1] << ' '
                  << borders[i].cond       << ' '
                  << borders[i].type       << ' ' << '\n';
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
    isError &= checkFile(fin, getLog("Error - params.txt"));
    fin >> _size.nodes
        >> _size.elems
        >> _size.areas
        >> _size.conds;
    fin.close();

    resize();                                                                   /// Выделение памяти под вектора
    std::fill_n(ig.begin(), 2, 0);                                              /// Заполнение первых 2 ячеек веткора ig нулями
                                                                                ///         (при желании можно поменять на единицы)
    fin.open(path / "nodes.txt");
    isError &= checkFile(fin, getLog("Error - nodes.txt"));
    for (size_t i = 0; i < _size.nodes; i++)
		fin >> nodes[i].x >> nodes[i].y;
    fin.close();

    fin.open(path / "elems.txt");
    isError &= checkFile(fin, getLog("Error - elems.txt"));
	for (size_t i = 0; i < _size.elems; i++) {
		fin >> elems[i].nodeIdx[0]
            >> elems[i].nodeIdx[1]
            >> elems[i].nodeIdx[2];
    }
    fin.close();

    fin.open(path / "areas.txt");
    isError &= checkFile(fin, getLog("Error - areas.txt"));
    for (size_t i = 0; i < _size.areas; i++)
        fin >> materials[i].lambda
            >> materials[i].gamma
            >> materials[i].betta;

    for (size_t i = 0; i < _size.elems; i++)
        fin >> elems[i].area;
    fin.close();

    fin.open(path / "bords.txt");
    isError &= checkFile(fin, getLog("Error - bords.txt"));
    for (size_t i = 0; i < _size.conds; i++)
        fin >> borders[i].area
            >> borders[i].bordIdx[0]
            >> borders[i].bordIdx[1]
            >> borders[i].cond
            >> borders[i].type;
    fin.close();
    return isError;
}

void FEM::writeFile(const std::filesystem::path& _path) const {
    std::filesystem::create_directories(_path);

}

void FEM::resize() {
    nodes.    resize(  _size.nodes  );                                              /// Выделение памяти для координат узлов
    elems.    resize(  _size.elems  );                                              /// Выделение памяти для хранение количества елементов
    borders.  resize(  _size.conds  );                                              /// Выделение памяти для хранения краевых условий
    materials.resize(  _size.areas  );                                              /// Выделение памяти для хранения материалов                                /// Выделение памяти для хранения индекса материала определенной области

    global_b. resize(  _size.nodes  );
    di.       resize(  _size.nodes  );
    ig.       resize(_size.nodes + 1);
}
#endif // _FEM_HPP_
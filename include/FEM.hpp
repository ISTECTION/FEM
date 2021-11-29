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
#include "utilites/Vector.hpp"
#include "Logger.hpp"

#include <filesystem>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <set>

#define _UNION_BEGIN namespace Union {
#define _UNION_END                   }

_UNION_BEGIN

struct XY { double x, y; };

struct Area {
    struct Material { double betta, gamma; };

    std::vector<Material> material;
    std::vector<size_t>   area    ;
};

struct Element {
    size_t area;
    std::array<size_t, 3> nodeIdx;
};

struct Border {
    size_t area;
    size_t cond;
    size_t type;
    std::array<size_t, 2> bordIdx;
};

struct Param {
    size_t nodes;                                                               /// Количество узлов
    size_t elems;                                                               /// Количество елементов
    size_t areas;                                                               /// Количество областей
    size_t conds;                                                               /// Количество краевых условий
};

_UNION_END

#undef _UNION_BEGIN
#undef _UNION_END


class FEM
{
private:
    Union::Param _size;
    double       _lambda;

    Union::Area materials;                                                      /// Структура материалов
    std::vector<Union::XY>       nodes;                                         /// Вектор узлов
    std::vector<Union::Element>  elems;                                         /// Вектор конечных элементов
    std::vector<Union::Border>   borders;                                       /// Вектор краевых условий


    std::vector<double> gg;
    std::vector<double> di;
    std::vector<size_t> ig;
    std::vector<size_t> jg;

public:
    FEM(std::filesystem::path _path) {

        assert(readFile(_path));                                                /// Читаем входные данные
        portrait(true);                                                         /// Создаём портрет
        writeFile("output" / _path.filename());                                 /// Записываем результаты

    }
    ~FEM() { }

    void printAll() const;

private:
    bool readFile(const std::filesystem::path& );
    void portrait(const bool isWriteList = false);

    void writeFile(const std::filesystem::path& ) const;
    void resize();
};

void FEM::portrait(const bool isWriteList) {
    const size_t N { _size.nodes };
    std::vector<std::set<size_t>> list(N);

    for (size_t el = 0; el < _size.elems; el++)
    for (size_t point = 0; point < 3; point++) {
        for (size_t i = point + 1; i < 3; i++) {
            size_t idx1 = { elems[el].nodeIdx[point] };
            size_t idx2 = { elems[el].nodeIdx[  i  ] };
            list[std::max(idx1, idx2)].insert(std::min(idx1, idx2));
        }
    }

    for (size_t i = 2; i < ig.size(); i++)
        ig[i] = ig[i - 1] + list[i - 1].size();

    jg.resize(ig[N]);                                                           /// Выделение памяти происходит в данном методе, т.к. размер
    gg.resize(ig[N]);                                                           /// векторов jd и gg равен последнему элементу вектора ig

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
    const size_t LENGTH = 20;
    const char symbol = '-';

    #define PRINTLINE for (size_t i = 0; i < LENGTH; std::cout << symbol, i++);
    #define ENDLINE   std::cout << '\n';
    system("chcp 65001");
    PRINTLINE ENDLINE
    std::cout << "PARAMS:         "                 << '\n';
    std::cout << "Size nodes:     " << _size.nodes  << '\n';
    std::cout << "Size element:   " << _size.elems  << '\n';
    std::cout << "Size areas:     " << _size.areas  << '\n';
    std::cout << "Size condition: " << _size.conds  << '\n';
    std::cout << "\u03BB               " << _lambda << '\n';
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
                  << materials.area[i]   << '\n';
    PRINTLINE ENDLINE
    std::cout << "Areas: " << '\n';
    for (size_t i = 0; i < _size.areas; i++) {
        std::cout << i << ' ' << '-' << ' '
                  << materials.material[i].gamma << ' '
                  << materials.material[i].betta << ' ' << '\n';
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
}

bool FEM::readFile(const std::filesystem::path& path) {
    using namespace ::Log;
    bool isError { true };

    std::ifstream fin(path / "params.txt");
    isError &= checkFile(fin, getLog("Error - params.txt"));
    fin >> _size.nodes
        >> _size.elems
        >> _size.areas
        >> _size.conds
        >> _lambda;
    fin.close();
    resize();                                                                   /// Выделение памяти под вектора

    fin.open(path / "nodes.txt");
    isError &= checkFile(fin, getLog("Error - nodes.txt"));
    for (size_t i = 0; i < _size.nodes; i++)
		fin >> nodes[i].x >> nodes[i].y;
    fin.close();

    fin.open(path / "elems.txt");
    isError &= checkFile(fin, getLog("Error - elems.txt"));
	for (size_t i = 0; i < _size.elems; i++) {
        elems[i].area = { i + 1 };
		fin >> elems[i].nodeIdx[0]
            >> elems[i].nodeIdx[1]
            >> elems[i].nodeIdx[2];
    }
    fin.close();

    fin.open(path / "areas.txt");
    isError &= checkFile(fin, getLog("Error - areas.txt"));
    for (size_t i = 0; i < _size.areas; i++)
        fin >> materials.material[i].gamma
            >> materials.material[i].betta;

    for (size_t i = 0; i < _size.elems; i++)
        fin >> materials.area[i];
    fin.close();

    fin.open(path / "borders.txt");
    isError &= checkFile(fin, getLog("Error - borders.txt"));
    for (size_t i = 0; i < _size.conds; i++)
        fin >> borders[i].area
            >> borders[i].bordIdx[0]
            >> borders[i].bordIdx[1]
            >> borders[i].cond
            >> borders[i].type;
    fin.close();
    return isError;
}



// di.resize(_size_nodes);
// x.resize(_size_nodes);

// r.resize(_size_nodes);
// p.resize(_size_nodes);
// z.resize(_size_nodes);
// q.resize(_size_nodes);
// f.resize(_size_nodes);

// fin.open(path / "ig.txt");
// for(size_t i = 0; i < ig.size(); i++)
//     fin >> ig[i];
// fin.close();

// gg.resize(ig.back());


void FEM::writeFile(const std::filesystem::path& _path) const {
    std::filesystem::create_directories(_path);

}

void FEM::resize() {
    nodes             .resize(_size.nodes);                                     /// Выделение памяти для координат узлов
    elems             .resize(_size.elems);                                     /// Выделение памяти для хранение количества елементов
    borders           .resize(_size.conds);                                     /// Выделение памяти для хранения краевых условий
    materials.material.resize(_size.areas);                                     /// Выделение памяти для хранения материалов
    materials.area    .resize(_size.elems);                                     /// Выделение памяти для хранения индекса материала определенной области

    ig.resize(_size.nodes + 1); ig[0] = ig[1] = 0;
    di.resize(_size.nodes    );
}
#endif // _FEM_HPP_
#ifndef _UNION_HPP_
#define _UNION_HPP_
#include <vector>
#include <array>

#define _UNION_BEGIN namespace Union {
#define _UNION_END                   }

_UNION_BEGIN

struct XY {
    double x;                                                                   /// Координаты узла по X
    double y;                                                                   /// Координаты узла по Y
    size_t area;                                                                /// Область координаты для аналитической функции
};

struct Material {
    double betta;                                                               /// Коэффициент betta
    double gamma;                                                               /// Коэффициент gamma
};

struct Element {
    size_t area;                                                                /// Область конечного элемента
    std::array<size_t, 3> nodeIdx;                                              /// Узлы конечного элемента
};

struct Boundary {
    size_t cond;                                                                /// Краевое условие
    size_t type;                                                                /// Тип краевого условия
    size_t area;                                                                /// Область границы
    std::array<size_t, 2> nodeIdx;                                              /// Узлы границы
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
#endif /// _UNION_HPP_
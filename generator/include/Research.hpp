#ifndef _RESEARCH_HPP_
#define _RESEARCH_HPP_

#include <iostream>
#include <vector>
#include <array>



struct XY     { double x, y;        };      /// node coordinates
struct Limits { double left, right; };
struct Element {
    size_t area;                            /// area element
    std::array<size_t, 3> nodeIdx;          /// node element
};
struct Area {
    Limits limits;
    size_t area;
};


class Research
{
private:
    Area X;
    Area Y;

    std::vector<XY>      nodes;
    std::vector<Element> elems;

    void generate();
public:
    Research(Area _X, Area _Y) : X(_X), Y(_Y) { generate(); }
    Research(
            double x_min, double x_max, size_t x_area,
            double y_min, double y_max, size_t y_area) {

        X.limits = { x_min, x_max };
        Y.limits = { y_min, y_max };
        X.area   = x_area;
        Y.area   = y_area;

        generate();
    }
};

void Research::generate() {
    nodes.resize((X.area + 1) * (Y.area + 1));
    elems.resize(X.area * Y.area * 2);


    for (size_t row = 0; row < Y.area; row++) {

        for (size_t col = 0; col < X.area; col++) {

            elems[0].nodeIdx[0] = 0; // stop
        }
    }


}
#endif // _RESEARCH_HPP_
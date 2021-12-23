#include <vector>
#include <array>

namespace Union {
    struct XY       { double x, y;         };
    struct Material { double betta, gamma; };

    struct Element {
        size_t area;
        std::array<size_t, 3> nodeIdx;
    };

    struct Boundary {
        size_t cond, type, area;
        std::array<size_t, 2> nodeIdx;
    };

    struct Param {
        size_t nodes;
        size_t elems;
        size_t areas;
        size_t conds;
    };
}
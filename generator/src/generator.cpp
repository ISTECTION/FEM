#include "../include/Research.hpp"

int main(int argc, char* argv[]) {

    const double X_MIN = 1;
    const double X_MAX = 9;
    const double Y_MIN = 1;
    const double Y_MAX = 3;

    size_t X_AREA = 4;
    size_t Y_AREA = 2;

    Research(X_MIN, X_MAX, X_AREA, Y_MIN, Y_MAX, Y_AREA);
    return 0;
}
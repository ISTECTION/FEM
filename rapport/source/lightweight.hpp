double
determinant(const std::array<Union::XY, 3>& elem) {
    return (
        (elem[1].x - elem[0].x) * (elem[2].y - elem[0].y) -
        (elem[1].y - elem[0].y) * (elem[2].x - elem[0].x)
    );
}

double
edgeLength(const std::array<Union::XY, 2>& elem) {
    return (
        sqrt (
            pow(elem[1].x - elem[0].x ,2) +
            pow(elem[1].y - elem[0].y, 2)
        )
    );
}
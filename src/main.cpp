#include "FEM.hpp"
#include <iostream>


int main(int argc, char* argv[]) {
    using namespace ::Log;

    if (argc != 2) assert(Logger::append(getLog("argc != 2")));
    FEM fem = FEM(argv[1]);
    fem.printAll();

    return 0;
}
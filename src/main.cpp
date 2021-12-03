#include "argparse/argparse.hpp"
#include "FEM.hpp"

int main(int argc, char* argv[]) {
    using namespace ::Log;

    argparse::ArgumentParser program("FEM", "1.0.0");
    program.add_argument("-i", "--input" ).required().help("path to input files" );
    program.add_argument("-o", "--output").required().help("path to output files");

    try {
        program.parse_args(argc, argv);
        FEM fem      (program.get<std::string>("-i"));
        fem.writeFile(program.get<std::string>("-o"));

        fem.printAll();
        fem.printSparse();
    }
    catch(const std::runtime_error& err) {
        Logger::append(getLog("argc != 3"));
        std::cerr << err.what();
        std::cerr << program;
        std::exit(1);
    }

    return 0;
}
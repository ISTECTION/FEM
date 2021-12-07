#include "argparse/argparse.hpp"
#include "timer/cxxtimer.hpp"
#include "LOS/LOS.hpp"
#include "FEM.hpp"

int main(int argc, char* argv[]) {
    using namespace ::Log;
    using ::std::chrono::milliseconds;

    argparse::ArgumentParser program("FEM", "1.0.0");       /// $\frac{a+b}{2}$
    program.add_argument("-i", "--input")
        .help("path to input files" )
        .required();

    program.add_argument("-o", "--output")
        .help("path to output files")
        .required();

    try {
        program.parse_args(argc, argv);

        cxxtimer::Timer timer(true);
        FEM fem      (program.get<std::string>("-i"));
        fem.writeFile(program.get<std::string>("-o"), 1E-14, 10000);
        LOS<double> l(program.get<std::string>("-o"));
        // l.solve(Cond::HOLLESKY, true);
        timer.stop();
        // l.printX();

        std::cout << '\n' << "Milliseconds: "
                  << timer.count<milliseconds>() << '\n';
        fem.printAll();
        fem.printSparse();
    }
    catch(const std::runtime_error& err) {
        Logger::append(getLog("argc != 3 (FEM -i input -o output)"));
        std::cerr << err.what();
        std::cerr << program;
        std::exit(1);
    }
    return 0;
}
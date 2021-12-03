#include "argparse/argparse.hpp"
#include "timer/cxxtimer.hpp"
#include "FEM.hpp"

int main(int argc, char* argv[]) {
    using namespace ::Log;
    using ::std::chrono::milliseconds;

    argparse::ArgumentParser program("FEM", "1.0.0");
    program.add_argument("-i", "--input" ).required().help("path to input files" );
    program.add_argument("-o", "--output").required().help("path to output files");

    try {
        program.parse_args(argc, argv);

        cxxtimer::Timer timer(true);
        FEM fem      (program.get<std::string>("-i"));
        fem.writeFile(program.get<std::string>("-o"), 1E-14, 10000);

        /// LOS
        timer.stop();
        std::cout << "Milliseconds: "
                  << timer.count<milliseconds>() << '\n';

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
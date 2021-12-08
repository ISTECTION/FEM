#include "argparse/argparse.hpp"
#include "timer/cxxtimer.hpp"
#include "LOS/LOS.hpp"
#include "FEM.hpp"

#include <optional>

int main(int argc, char* argv[]) {
    using namespace ::Log;
    using ::std::chrono::milliseconds;

    argparse::ArgumentParser _program("FEM", "1.0.0");
    _program.add_argument("-i", "--input")
        .help("path to input files" )
        .required();

    _program.add_argument("-o", "--output")
        .help("path to output files");

    try {
        _program.parse_args(argc, argv);

        std::optional _opt            = _program.present("-o");
        std::filesystem::path _input  = _program.get<std::string>("-i");
        std::filesystem::path _output =
            _opt.has_value() ?
                _program.get<std::string>("-o") :
                _input / "sparse";

        cxxtimer::Timer timer(true);

        FEM fem(_input);
        fem.writeFile(_output, 1E-14, 10000);
        LOS<double> los(_output);
        los.solve(Cond::HOLLESKY, true);

        timer.stop();
        std::cout << "Milliseconds: "
                  << timer.count<milliseconds>();

        los.printX();       /// print solution vector
        fem.printAll();     /// print input FEM data
        fem.printSparse();  /// print input FEM data (sparse format)

    } catch(const std::runtime_error& err) {

        Logger::append(getLog("argc != 3 (FEM -i input -o output)"));
        std::cerr << err.what();
        std::cerr << _program;
        std::exit(1);       /// program termination with error

    }
    return 0;
}
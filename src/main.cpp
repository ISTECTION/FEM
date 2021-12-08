#include "argparse/argparse.hpp"
#include "timer/cxxtimer.hpp"
#include "LOS/LOS.hpp"
#include "FEM.hpp"

#include <optional>

int main(int argc, char* argv[]) {
    using namespace    ::Log;
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

        cxxtimer::Timer _timer(true);
        FEM _FEM(_input);

        _FEM.printAll();        /// print input FEM data
        _FEM.printSparse();     /// print input FEM data (sparse format)

        LOS<double> _LOS (
            _FEM.takeDate(),    /// data
            _FEM.getNodes(),    /// count nodes
            1E-16, 1000 );      /// epsilon and max iteration
        _LOS.solve(Cond::DIAGONAL, true);

        _timer.stop();
        _LOS.printX();          /// print solution vector

        std::cout << "Milliseconds: " <<
            _timer.count<milliseconds>();

    } catch(const std::runtime_error& err) {

        Logger::append(getLog("argc != 2 (FEM --input ./input)"));
        std::cerr << err.what();
        std::cerr << _program;
        std::exit(1);       /// program termination with error

    }
    return 0;
}
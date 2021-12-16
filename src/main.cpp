#include "argparse/argparse.hpp"
#include "timer/cxxtimer.hpp"
#include "LOS/LOS.hpp"
#include "FEM.hpp"

#include <iostream>
#include <optional>
#include <fstream>

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

        Function::setFunction(_input.string());

        cxxtimer::Timer _timer(true);           /// start timer
        FEM _FEM(_input);                       /// start FEM
        _FEM.writeFile(_output, 1E-20, 10000);  /// overwriting files
        LOS<double> _LOS(_output);              /// here is reading
        _LOS.solve(Cond::NONE, true);
        _timer.stop();                          /// stop timer

        #if DEBUG != 0
        _FEM.printAll();                        /// print input FEM data
        _FEM.printSparse();                     /// print sparse format
        _FEM.printAnalitics();                  /// print analicals solve
        #endif

        std::cout << "SOLUTION: ";
        print(_LOS.getX(), 14);                 /// print solution vector
        std::cout << "Milliseconds: " << _timer.count<milliseconds>();




        std::cout << std::endl << _FEM.getValue(1.125, 1.125, _LOS.getX()) << std::endl;

    } catch(const std::runtime_error& err) {
        Logger::append(getLog("argc != 2 (FEM --input ./input)"));
        std::cerr << err.what();
        std::cerr << _program;
        std::exit(1);                           /// program error
    }

    return 0;
}
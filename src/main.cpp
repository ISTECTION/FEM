#include "argparse/argparse.hpp"
#include "timer/cxxtimer.hpp"
#include "FEM.hpp"

#include <iostream>
#include <optional>
#include <fstream>

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    using namespace    ::Log;
    using ::std::chrono::milliseconds;

    argparse::ArgumentParser _prs("FEM", "1.0.0");
    _prs.add_argument("-i", "--input")
        .help( "path to input files" )
        .required();

    _prs.add_argument("-o", "--output")
        .help( "path to output files" );

    try {
        using std::string;

        _prs.parse_args(argc, argv);
        std::optional         _opt = _prs.present    ("-o");
        std::filesystem::path _inp = _prs.get<string>("-i");
        std::filesystem::path _out = _opt.has_value()
                                        ? _prs.get<string>("-o")
                                        : _inp / "sparse";

        Function::setFunction(_inp.string());       /// set function

        cxxtimer::Timer _timer(true);               /// start timer
        FEM _FEM(_inp);                             /// start FEM
        _FEM.startLOS(_out);
        _timer.stop();                              /// stop timer

        #if DEBUG != 0                              /// printing
        _FEM.printAll();                            /// print input FEM data
        _FEM.printSparse();                         /// print sparse format
        _FEM.createTable();
        #endif

        _FEM(1.125, 1.125);
        _FEM(1.5, 2);
        _FEM(3,   4);

        std::cout << _timer;                        /// print time
    } catch(const std::runtime_error& err) {
        #define ARGUMENTS_NO_RECEIVED 2
        Logger::append(getLog(
            "argc != 2 (FEM --input ./input)"
        ));
        std::cerr << err.what() << std::endl;
        std::cerr <<    _prs    << std::endl;
        std::exit(  ARGUMENTS_NO_RECEIVED  );
    }
    return 0;
}
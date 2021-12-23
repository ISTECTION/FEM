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

    _prs.add_argument("-d", "--debug")
        .help("debugging information")
        .default_value(false)
        .implicit_value(true);

    try {
        using std::string;

        _prs.parse_args(argc, argv);
        std::optional         _opt = _prs.present    ("-o");
        std::filesystem::path _inp = _prs.get<string>("-i");
        std::filesystem::path _out = _opt.has_value()
                                        ? _prs.get<string>("-o")
                                        : _inp / "sparse";

        Function::setFunction(_inp.string());
        cxxtimer::Timer _timer(true);
        FEM _FEM(_inp, _prs.get<bool>("-d"));
        _FEM.startLOS(_out);
        _timer.stop();

        if (_prs["-d"] == true) {
            _FEM.printAll();
            _FEM.printSparse();
            _FEM.createTable();
        } else
            _FEM.printZ();

        std::cout << _timer;
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
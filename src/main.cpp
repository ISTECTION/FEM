#include "timer/cxxtimer.hpp"
#include "FEM.hpp"

#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    using ::std::chrono::milliseconds;
    using namespace    ::Log;

    init_argv();

    try {
        using std::string;

        _prs.parse_args(argc, argv);
        std::optional         _opt = _prs.present    ("-o");
        std::filesystem::path _inp = _prs.get<string>("-i");
        std::filesystem::path _out = _opt.has_value()
                                        ? _prs.get<string>("-o")
                                        : _inp / "sparse";

        cxxtimer::Timer _timer(true);                                           /// start timer
        FEM _FEM(_inp, _prs.get<bool>("-d"));                                   /// start FEM
        _FEM.startLOS(_out);
        _timer.stop();                                                          /// stop timer

        if (_prs["-d"] == true) {                                               /// printing
            _FEM.printAll();                                                    /// print input FEM data
            _FEM.printSparse();                                                 /// print sparse format
            _FEM.createTable();
        } else
            _FEM.printZ();

        /// Value at the point
        // _FEM(1.125, 1.125);
        // _FEM(1.5, 2);
        // _FEM(3,   4);

        std::cout << _timer;                                                    /// print time
    } catch(const std::runtime_error& err) {
        constexpr uint8_t ARGUMENTS_NO_RECEIVED = 2;
        Logger::append(getLog(
            "argc != 2 (FEM --input ./input)"
        ));
        std::cerr << err.what() << std::endl;
        std::cerr <<    _prs    << std::endl;
        std::exit(  ARGUMENTS_NO_RECEIVED  );
    }
    return 0;
}
#ifndef _ARGV_HPP_
#define _ARGV_HPP_
#include "argparse/argparse.hpp"

static argparse::ArgumentParser _prs("FEM", "1.0.0");

void init_argv() {
    _prs.add_argument("-i", "--input")
        .help( "path to input files" )
        .required();

    _prs.add_argument("-o", "--output")
        .help( "path to output files" );

    _prs.add_argument("-d", "--debug")
        .help("debugging information")
        .default_value(false)
        .implicit_value(true);

    _prs.add_argument("--indicator")
        .help("loading indicator")
        .default_value(false)
        .implicit_value(true);
}
#endif /// _ARGV_HPP_
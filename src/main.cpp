#include <string>
#include <iterator>
#include <iostream>

#include "cli.hpp"
#include "beeline.hpp"


std::string read_all_from(std::istream& input)
{
    input >> std::noskipws;
    std::istream_iterator<char> it(input);
    std::istream_iterator<char> end;
    return {it, end};
}


int main(const int argc, const char** argv)
{
    Arguments arguments = ArgumentParser().parse(argc, argv);
    init_logging(arguments.logging_level);
    Beeline{}.run(read_all_from(std::cin));
}

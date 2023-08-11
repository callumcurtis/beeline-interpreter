#include <string>
#include <iterator>
#include <iostream>

#include "cli.hpp"
#include "beeline.hpp"


// Reads all characters from the given input stream.
// Returns a string containing all characters read.
std::string read_all_from(std::istream& input)
{
    input >> std::noskipws;
    std::istream_iterator<char> it(input);
    std::istream_iterator<char> end;
    return {it, end};
}


// Reads all characters from stdin and runs the beeline
// interpreter on the input. Sets the logging level
// according to the given arguments. Returns 0 on success
// and 1 on error.
int main(const int argc, const char** argv)
{
    Arguments arguments = ArgumentParser().parse(argc, argv);
    init_logging(arguments.logging_level);
    int return_code = 0;
    try
    {
        Beeline{}.run(read_all_from(std::cin));
    }
    catch (const BeelineError& be)
    {
        return_code = 1;
    }
    return return_code;
}

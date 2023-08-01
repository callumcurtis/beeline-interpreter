#include "cli.hpp"


int main(const int argc, const char** argv)
{
    Arguments arguments = ArgumentParser().parse(argc, argv);
    init_logging(arguments.logging_level);
}

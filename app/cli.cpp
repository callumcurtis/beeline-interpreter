#include <sstream>
#include <iostream>
#include <memory>

#include <boost/program_options.hpp>

#include "cli.hpp"


namespace po = boost::program_options;


// Builds a string containing the usage information for the given process.
std::string build_usage_string(const std::string &process_name, const po::options_description &desc)
{
    std::stringstream ss;
    ss << "usage: " << process_name << " [options]\n" << desc;
    return ss.str();
}


// Context used in the argument handler chain. Contains the original
// argc and argv, as well as the options description used to parse
// the arguments.
struct ArgumentParsingContext
{
    int argc;
    const char** argv;
    po::options_description &desc;
};


class ArgumentHandler
{
public:
    // Sets the next handler in the chain.
    // The next handler will be called after this handler has finished.
    void set_next(std::unique_ptr<ArgumentHandler> &&next)
    {
        this->next_ = std::move(next);
    }
    void handle(const Arguments arguments, const ArgumentParsingContext context) const
    {
        handle_(arguments, context);
        if (next_)
        {
            next_->handle(arguments, context);
        }
    }
private:
    std::unique_ptr<ArgumentHandler> next_{nullptr};
protected:
    // Overriden by subclasses to implement the actual handling.
    virtual void handle_(const Arguments arguments, const ArgumentParsingContext context) const = 0;
};


// Ensures the help and version flags are not set at the same time.
class HelpXorVersionValidationHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ArgumentParsingContext context) const override
    {
        if (arguments.version && arguments.help)
        {
            std::cerr << "error: version and help are mutually exclusive\n" << build_usage_string(context.argv[0], context.desc);
            exit(1);
        }
    }
};


// Handles the version flag if it is set by printing the version and exiting.
class VersionHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ArgumentParsingContext context) const override
    {
        if (arguments.version)
        {
            std::cout << "version: 0.0.1\n";
            exit(0);
        }
    }
};


// Handles the help flag if it is set by printing the usage string and exiting.
class HelpHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ArgumentParsingContext context) const override
    {
        if (arguments.help)
        {
            std::cout << build_usage_string(context.argv[0], context.desc);
            exit(0);
        }
    }
};


// Ensures the logging level is between 0 and 5.
class LoggingLevelValidationHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ArgumentParsingContext context) const override
    {
        if (arguments.logging_level < LoggingLevel::TRACE || arguments.logging_level > LoggingLevel::FATAL)
        {
            std::cerr << "error: logging level must be between 0 and 5\n" << build_usage_string(context.argv[0], context.desc);
            exit(1);
        }
    }
};


// Parses the arguments and returns an Arguments object.
class ArgumentParser::Impl
{
public:
    Impl()
    {
        // Build the chain of responsibility. The order of handlers is important.
        // Help and version must be checked for mutual exclusivity before
        // the help and version handlers are called.
        std::unique_ptr<HelpXorVersionValidationHandler> mutual_exclusive_help_and_version_handler = std::make_unique<HelpXorVersionValidationHandler>();
        std::unique_ptr<LoggingLevelValidationHandler> logging_level_validation_handler = std::make_unique<LoggingLevelValidationHandler>();
        std::unique_ptr<HelpHandler> help_handler = std::make_unique<HelpHandler>();
        std::unique_ptr<VersionHandler> version_handler = std::make_unique<VersionHandler>();

        // Set the next handler in the chain. Reverse order is necessary
        // to ensure handlers are not referenced after they are moved.
        help_handler->set_next(std::move(version_handler));
        logging_level_validation_handler->set_next(std::move(help_handler));
        mutual_exclusive_help_and_version_handler->set_next(std::move(logging_level_validation_handler));

        handler_chain_ = std::move(mutual_exclusive_help_and_version_handler);
    }
    Arguments parse(const int argc, const char** argv) const
    {
        po::options_description desc = build_options_description();
        po::variables_map vm;

        try
        {
            po::store(po::parse_command_line(argc, argv, desc), vm);
        }
        catch (const po::error &e)
        {
            std::cerr << "error: " << e.what() << "\n" << build_usage_string(argv[0], desc);
            exit(1);
        }

        po::notify(vm);

        const Arguments arguments = {
            static_cast<LoggingLevel>(vm["debug_level"].as<int>()),
            vm.count("version") > 0,
            vm.count("help") > 0,
        };

        handler_chain_->handle(arguments, {argc, argv, desc});

        return arguments;
    }
private:
    std::unique_ptr<ArgumentHandler> handler_chain_;
    po::options_description build_options_description() const
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("debug_level,d", po::value<int>()->default_value(4), "set debug level (0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=fatal)")
            ("help,h", "produce help message")
            ("version,v", "print version string")
        ;
        return desc;
    }
};


ArgumentParser::ArgumentParser() : impl_(std::make_unique<Impl>()) {}
ArgumentParser::~ArgumentParser() = default;
Arguments ArgumentParser::parse(const int argc, const char** argv) { return impl_->parse(argc, argv); }

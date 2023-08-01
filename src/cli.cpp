#include <sstream>
#include <iostream>
#include <memory>

#include <boost/program_options.hpp>

#include "cli.hpp"


namespace po = boost::program_options;


std::string build_usage_string(const std::string &process_name, const po::options_description &desc)
{
    std::stringstream ss;
    ss << "usage: " << process_name << " [options]\n" << desc;
    return ss.str();
}


struct ParsingContext
{
    const int argc;
    const char** argv;
    const po::options_description &desc;
};


class ArgumentHandler
{
public:
    void set_next(std::unique_ptr<ArgumentHandler> &&next)
    {
        this->next_ = std::move(next);
    }
    void handle(const Arguments arguments, const ParsingContext context) const
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
    virtual void handle_(const Arguments arguments, const ParsingContext context) const = 0;
};


class HelpXorVersionValidationHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ParsingContext context) const override
    {
        if (arguments.version && arguments.help)
        {
            std::cerr << "error: version and help are mutually exclusive\n" << build_usage_string(context.argv[0], context.desc);
            exit(1);
        }
    }
};


class VersionHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ParsingContext context) const override
    {
        if (arguments.version)
        {
            std::cout << "version: 0.0.1\n";
            exit(0);
        }
    }
};


class HelpHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ParsingContext context) const override
    {
        if (arguments.help)
        {
            std::cout << build_usage_string(context.argv[0], context.desc);
            exit(0);
        }
    }
};


class LogLevelValidationHandler : public ArgumentHandler
{
protected:
    void handle_(const Arguments arguments, const ParsingContext context) const override
    {
        if (arguments.log_level < LogLevel::TRACE || arguments.log_level > LogLevel::FATAL)
        {
            std::cerr << "error: log level must be between 0 and 5\n" << build_usage_string(context.argv[0], context.desc);
            exit(1);
        }
    }
};


class ArgumentParser::Impl
{
public:
    Impl()
    {
        std::unique_ptr<HelpXorVersionValidationHandler> mutual_exclusive_help_and_version_handler = std::make_unique<HelpXorVersionValidationHandler>();
        std::unique_ptr<LogLevelValidationHandler> log_level_validation_handler = std::make_unique<LogLevelValidationHandler>();
        std::unique_ptr<HelpHandler> help_handler = std::make_unique<HelpHandler>();
        std::unique_ptr<VersionHandler> version_handler = std::make_unique<VersionHandler>();

        help_handler->set_next(std::move(version_handler));
        log_level_validation_handler->set_next(std::move(help_handler));
        mutual_exclusive_help_and_version_handler->set_next(std::move(log_level_validation_handler));

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
            static_cast<LogLevel>(vm["log_level"].as<int>()),
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
            ("log_level,d", po::value<int>()->default_value(0), "set log level")
            ("help,h", "produce help message")
            ("version,v", "print version string")
        ;
        return desc;
    }
};


ArgumentParser::ArgumentParser()
{
    impl_ = new Impl();
}


ArgumentParser::~ArgumentParser()
{
    delete impl_;
}


Arguments ArgumentParser::parse(const int argc, const char** argv)
{
    return impl_->parse(argc, argv);
}

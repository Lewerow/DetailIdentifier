#include <executor/executor.h>
#include <boost/program_options.hpp>

#include <preprocessor/options.h>
#include <executor/options.h>
#include <executor/version.h>
#include <executor/configurator.h>
#include <executor/application.h>

#include <logger/logger.h>
#include <logger/command_line_logger.h>

#include <numeric>

namespace
{
	boost::program_options::options_description get_all_options()
	{
		boost::program_options::options_description all_options;
		all_options.add(executor::setup_options());
		all_options.add(preprocessor::setup_options());

		return all_options;
	}

	boost::program_options::variables_map setup_options(int argc, char** argv, const boost::program_options::options_description& all_options, logger::logger& log)
	{
		boost::program_options::variables_map vars;
		
		int parser_style = boost::program_options::command_line_style::default_style ^ boost::program_options::command_line_style::allow_guessing;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, all_options, parser_style), vars);
		
		try
		{
			boost::program_options::store(boost::program_options::parse_config_file<char>(vars.at("config_file").as<std::string>().c_str(), all_options), vars);
		}
		catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::program_options::reading_file> > ex)
		{
			log.warning_log(std::string("Error while reading configuration from file: ") + ex.what() + ".");
			log.info_log("Proceeding with startup");
		}

		return vars;
	}

	boost::program_options::variables_map setup_parameters(int argc, char** argv, const boost::program_options::options_description& all_options, logger::logger& log)
	{
		try
		{
			boost::program_options::variables_map app_options = setup_options(argc, argv, all_options, log);
			return app_options;
		}
		catch (boost::program_options::ambiguous_option opt)
		{
			log.error_log(std::string("Ambigous option: ") + opt.what() + ".");
			log.info_log("Terminating");
			exit(1);
		}
		catch (boost::program_options::unknown_option opt)
		{
			log.error_log(std::string("Unknown option: ") + opt.what() + ".");
			exit(2);
		}
		catch (boost::program_options::invalid_command_line_syntax syn)
		{
			log.error_log(std::string("Invalid command line syntax: ") + syn.what() + ".");
			log.info_log("Terminating");
			exit(3);
		}
	}
}

namespace executor
{
    int executor::execute(int argc, char** argv)
	{
		logger::command_line_logger log;
		boost::program_options::options_description all_options(get_all_options());
		boost::program_options::variables_map params(setup_parameters(argc, argv, all_options, log));

		bool all_defaulted = true;
		for (auto& v : params)
			all_defaulted = all_defaulted && v.second.defaulted();

		if (params.count("help") || all_defaulted)
			std::cout << all_options;
		else if (params.count("version"))
			std::cout << APPLICATION_NAME << " v." << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_SUBMINOR << std::endl;
		else
			return configurator(params, log).application().run();

        return 0;
    }
}

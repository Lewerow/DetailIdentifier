#include <modeller/configuration.h>

#include <boost/program_options/variables_map.hpp>
#include <logger/logger.h>

namespace modeller
{
	class configuration::impl
	{
	public:
		impl(logger::logger& log_) : log(log_)
		{}

		std::string output_filename;
		std::string workspace_path;
		logger::logger& log;

		impl& operator=(impl&) = delete;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log) : pimpl(std::make_unique<impl>(log))
	{
		pimpl->output_filename = vars.at("output").as<std::string>();
		pimpl->workspace_path = vars.at("working_directory").as<std::string>();
	}

	configuration::~configuration()
	{}

	std::string configuration::output_filename() const
	{
		return pimpl->output_filename;
	}

	std::string configuration::workspace_path() const
	{
		return pimpl->workspace_path;
	}

	logger::logger& configuration::log() const
	{
		return pimpl->log;
	}
	
}
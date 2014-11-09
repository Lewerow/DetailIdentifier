#include <preprocessor/configuration.h>

#include <boost/program_options/variables_map.hpp>
#include <logger/logger.h>

namespace preprocessor
{
	class configuration::impl
	{
	public:
		impl(logger::logger& log_) : log(log_)
		{}

		std::string input_filename;
		std::string workspace_path;
		logger::logger& log;

		impl& operator=(impl&) = delete;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log) : pimpl(std::make_unique<impl>(log))
	{
		pimpl->input_filename = vars.at("input").as<std::string>();
		pimpl->workspace_path = vars.at("working_directory").as<std::string>();
	}

	configuration::~configuration()
	{}

	std::string configuration::input_filename() const
	{
		return pimpl->input_filename;
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
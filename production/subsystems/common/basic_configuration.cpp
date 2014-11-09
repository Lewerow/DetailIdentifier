#include <common/basic_configuration.h>
#include <boost/program_options/variables_map.hpp>

namespace common
{
	class basic_configuration::impl
	{
	public:
		impl(logger::logger* log_) : log(log_)
		{}

		std::string workspace_path;
		logger::logger* log;
	};

	basic_configuration::basic_configuration(const boost::program_options::variables_map& vars, logger::logger& log) : pimpl(std::make_unique<impl>(&log))
	{
		pimpl->workspace_path = vars.at("working_directory").as<std::string>();
	}

	basic_configuration::~basic_configuration()
	{}

	std::string basic_configuration::workspace_path() const
	{
		return pimpl->workspace_path;
	}

	logger::logger& basic_configuration::log() const
	{
		return *pimpl->log;
	}
}
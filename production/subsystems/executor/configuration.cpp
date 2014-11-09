#include <executor/configuration.h>

#include <boost/program_options/variables_map.hpp>
#include <logger/logger.h>

namespace executor
{
	class configuration::impl
	{
	public:
		std::string input_filename;
		std::string output_filename;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log) : basic_configuration(vars, log), pimpl(std::make_unique<impl>())
	{
		pimpl->input_filename = vars.at("input").as<std::string>();
		pimpl->input_filename = vars.at("output").as<std::string>();
	}

	configuration::~configuration()
	{}

	std::string configuration::input_filename() const
	{
		return pimpl->input_filename;
	}
	
	std::string configuration::output_filename() const
	{
		return pimpl->output_filename;
	}

}
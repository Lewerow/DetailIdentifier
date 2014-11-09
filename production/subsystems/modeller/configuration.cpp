#include <modeller/configuration.h>

#include <boost/program_options/variables_map.hpp>
#include <logger/logger.h>

namespace modeller
{
	class configuration::impl
	{
	public:
		std::string output_filename;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log) : basic_configuration(vars, log), pimpl(std::make_unique<impl>())
	{
		pimpl->output_filename = vars.at("output").as<std::string>();
	}

	configuration::~configuration()
	{}

	std::string configuration::output_filename() const
	{
		return pimpl->output_filename;
	}
}
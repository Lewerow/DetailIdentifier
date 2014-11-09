#include <interpreter/configuration.h>

#include <boost/program_options/variables_map.hpp>
#include <logger/logger.h>

namespace interpreter
{
	class configuration::impl
	{};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log) : basic_configuration(vars, log), pimpl(std::make_unique<impl>())
	{}

	configuration::~configuration()
	{}
}
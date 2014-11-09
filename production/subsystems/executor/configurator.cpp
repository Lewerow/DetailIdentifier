#include <executor/configurator.h>
#include <executor/application.h>

#include <preprocessor/configuration.h>

#include <boost/program_options/variables_map.hpp>

namespace executor
{
	configurator::configurator(const boost::program_options::variables_map& vars, logger::logger& log)
	{
		preprocessor::configuration config(vars, log);
		app = std::make_unique<executor::whole_application>();
	}

	configurator::~configurator()
	{}

	executor::application& configurator::application() const
	{
		return *app;
	}
}
#include <executor/configurator.h>
#include <executor/application.h>

#include <boost/program_options/variables_map.hpp>

namespace executor
{
	configurator::configurator(const boost::program_options::variables_map& vars)
	{}

	configurator::~configurator()
	{}

	executor::application& configurator::application() const
	{
		return *app;
	}
}
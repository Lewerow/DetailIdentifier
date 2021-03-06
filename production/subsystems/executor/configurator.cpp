#include <executor/configurator.h>
#include <executor/application.h>
#include <executor/os_proxy.h>

#include <preprocessor/configuration.h>
#include <vectorizer/configuration.h>
#include <interpreter/configuration.h>
#include <modeller/configuration.h>
#include <executor/configuration.h>

#include <boost/program_options/variables_map.hpp>

namespace executor
{
	configurator::configurator(const boost::program_options::variables_map& vars, logger::logger& log)
	{
		auto os = std::make_shared<os_proxy>();
		app_ = std::make_unique<executor::whole_application>(std::make_shared<preprocessor::configuration>(vars, log), 
			std::make_shared<vectorizer::configuration>(vars, log, os),
			std::make_shared<interpreter::configuration>(vars, log, os),
			std::make_shared<modeller::configuration>(vars, log),
			std::make_shared<executor::configuration>(vars, log));
	}

	configurator::~configurator()
	{}

	executor::application& configurator::app() const
	{
		return *app_;
	}
}

#include <helpers/simple_configuration.h>
#include <executor/executor.h>

namespace helpers
{
	boost::program_options::variables_map get_minimal_complete_configuration()
	{
		return configuration_from_test_file("minimal_complete_configuration.ini");
	}

	boost::program_options::variables_map configuration_from_test_file(const std::string& name)
	{
		boost::program_options::variables_map config;
		boost::program_options::store(boost::program_options::parse_config_file<char>((TEST_FILES_DIRECTORY_STRING + name).c_str(),
			executor::executor::all_application_options()), config);

		return config;
	}
}
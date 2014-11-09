#include <helpers/minimal_configuration.h>
#include <executor/executor.h>

namespace helpers
{
	boost::program_options::variables_map get_minimal_complete_configuration()
	{
		boost::program_options::variables_map config; 
		boost::program_options::store(boost::program_options::parse_config_file<char>((std::string(TEST_ROOT_DIRECTORY) + "/files/minimal_complete_configuration.ini").c_str(),
			executor::executor::all_application_options()), config);
	
		return config;
	}
}
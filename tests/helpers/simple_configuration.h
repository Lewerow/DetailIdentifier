#ifndef MINIMAL_CONFIGURATION_H_KDWIqmdiweofweiufjeiwrj59gj4g8huredkfsokwijwq9idju23fh34ufg9438fjerfkdf
#define MINIMAL_CONFIGURATION_H_KDWIqmdiweofweiufjeiwrj59gj4g8huredkfsokwijwq9idju23fh34ufg9438fjerfkdf

#include <string>
#include <boost/program_options/variables_map.hpp>

namespace helpers
{
	boost::program_options::variables_map get_minimal_complete_configuration();
	boost::program_options::variables_map configuration_from_test_file(const std::string& name);
}

#endif
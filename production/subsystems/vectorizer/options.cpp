#include <vectorizer/options.h>
#include <boost/program_options.hpp>

#include <string>

namespace vectorizer
{
	boost::program_options::options_description setup_options()
	{
		boost::program_options::options_description desc("Vectorization options");
		desc.add_options()
			("potrace", boost::program_options::value<std::string>()->value_name("POTRACE_LOCATION")->default_value(DEFAULT_POTRACE_EXECUTABLE_NAME_STRING),
				"Path to Potrace executable that should be used during vectorization")
			("mkbitmap", boost::program_options::value<std::string>()->value_name("MKBITMAP_LOCATION")->default_value(DEFAULT_MKBITMAP_EXECUTABLE_NAME_STRING),
				"Path to Mkbitmap executable that should be used during vectorization")
			;

		return desc;
	}
}
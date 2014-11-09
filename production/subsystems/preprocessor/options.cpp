#include <preprocessor/options.h>
#include <boost/program_options.hpp>

namespace preprocessor
{
	boost::program_options::options_description setup_options()
	{
		boost::program_options::options_description desc("Preprocessing options");

		return desc;
	}
}
#ifndef EXECUTOR_CONFIGURATION_FOIEJf9ejw98fjerfjkdmcxlxdk0923j4fuierjndfkljweorifrjhneijkdewdwqdwqd
#define EXECUTOR_CONFIGURATION_FOIEJf9ejw98fjerfjkdmcxlxdk0923j4fuierjndfkljweorifrjhneijkdewdwqdwqd

#include <common/basic_configuration.h>

namespace executor
{
	class configuration : public common::basic_configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();

		std::string input_filename() const;
		std::string output_filename() const;
	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
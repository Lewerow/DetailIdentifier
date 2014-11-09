#ifndef INTERPRETER_CONFIGURATION_H_fiosadsadqfsdcsadsadasd2r544tgrr34f34fwqdqwdwqdwqdfrdfgvgvcxvfbhgffhfdj43if43
#define INTERPRETER_CONFIGURATION_H_fiosadsadqfsdcsadsadasd2r544tgrr34f34fwqdqwdwqdwqdfrdfgvgvcxvfbhgffhfdj43if43

#include <common/basic_configuration.h>

namespace interpreter
{
	class configuration : public common::basic_configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();

	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
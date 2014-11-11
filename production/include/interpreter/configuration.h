#ifndef INTERPRETER_CONFIGURATION_H_fiosadsadqfsdcsadsadasd2r544tgrr34f34fwqdqwdwqdwqdfrdfgvgvcxvfbhgffhfdj43if43
#define INTERPRETER_CONFIGURATION_H_fiosadsadqfsdcsadsadasd2r544tgrr34f34fwqdqwdwqdwqdfrdfgvgvcxvfbhgffhfdj43if43

#include <common/basic_configuration.h>

namespace executor
{
	class os_proxy;
}

namespace interpreter
{
	class configuration : public common::basic_configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&, std::shared_ptr<executor::os_proxy>);
		~configuration();

		void set_os_proxy(std::shared_ptr<executor::os_proxy>);

		executor::os_proxy& os_proxy() const;
	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
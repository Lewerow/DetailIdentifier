#ifndef INTERPRETER_CONFIGURATION_H_fiosadsadqfsdcsadsadasd2r544tgrr34f34fwqdqwdwqdwqdfrdfgvgvcxvfbhgffhfdj43if43
#define INTERPRETER_CONFIGURATION_H_fiosadsadqfsdcsadsadasd2r544tgrr34f34fwqdqwdwqdwqdfrdfgvgvcxvfbhgffhfdj43if43

#include <memory>
#include <string>

namespace boost
{
	namespace program_options
	{
		class variables_map;
	}
}

namespace logger
{
	class logger;
}

namespace interpreter
{
	class configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();

		std::string workspace_path() const;
		logger::logger& log() const;
	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
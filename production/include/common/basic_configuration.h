#ifndef BASIC_COMMON_CONFIGURATION_H_DSAMoidm34orgmefodfmclskdm2dfjk34iokfnrjeklsjkdwqemdifjenflierjfowen
#define BASIC_COMMON_CONFIGURATION_H_DSAMoidm34orgmefodfmclskdm2dfjk34iokfnrjeklsjkdwqemdifjenflierjfowen

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

namespace common
{
	class basic_configuration
	{
		class impl;
	public:
		basic_configuration(const boost::program_options::variables_map&, logger::logger&);
		~basic_configuration();


		std::string workspace_path() const;
		logger::logger& log() const;

	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
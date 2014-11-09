#ifndef PREPROCESSOR_CONFIGURATION_H_fioewfm4guufkdmslfwemfi34mfgunijtrkgofr34f34fj43if43
#define PREPROCESSOR_CONFIGURATION_H_fioewfm4guufkdmslfwemfi34mfgunijtrkgofr34f34fj43if43

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

namespace preprocessor
{
	class configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();

		std::string input_filename() const;
		std::string workspace_path() const;
		logger::logger& log() const;
	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
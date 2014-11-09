#ifndef MODDELLER_CONFIGURATION_H_fioasdaedqwdwwqdfqwdmwq4dgwqduwqur43fr5k345d34m5s435l43f5w2423emfi34mfgunijtrkgofr34f34fj43if43
#define MODDELLER_CONFIGURATION_H_fioasdaedqwdwwqdfqwdmwq4dgwqduwqur43fr5k345d34m5s435l43f5w2423emfi34mfgunijtrkgofr34f34fj43if43

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

namespace modeller
{
	class configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();

		std::string output_filename() const;
		std::string workspace_path() const;
		logger::logger& log() const;
	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
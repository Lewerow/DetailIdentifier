#ifndef MODDELLER_CONFIGURATION_H_fioasdaedqwdwwqdfqwdmwq4dgwqduwqur43fr5k345d34m5s435l43f5w2423emfi34mfgunijtrkgofr34f34fj43if43
#define MODDELLER_CONFIGURATION_H_fioasdaedqwdwwqdfqwdmwq4dgwqduwqur43fr5k345d34m5s435l43f5w2423emfi34mfgunijtrkgofr34f34fj43if43

#include <common/basic_configuration.h>

namespace modeller
{
	class configuration : public common::basic_configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();

		std::string output_filename() const;
	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
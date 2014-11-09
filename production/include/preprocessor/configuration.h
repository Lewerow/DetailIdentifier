#ifndef PREPROCESSOR_CONFIGURATION_H_fioewfm4guufkdmslfwemfi34mfgunijtrkgofr34f34fj43if43
#define PREPROCESSOR_CONFIGURATION_H_fioewfm4guufkdmslfwemfi34mfgunijtrkgofr34f34fj43if43

#include <common/basic_configuration.h>

namespace preprocessor
{
	class configuration : public common::basic_configuration
	{
		class impl;
	public:
		configuration(const boost::program_options::variables_map&, logger::logger&);
		~configuration();
		
		void set_input_filename(const std::string& path);
		std::string input_filename() const;

		configuration(const configuration&) = delete;
	protected:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
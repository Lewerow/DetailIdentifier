#ifndef VECTORIZER_CONFIGURATION_H_KFOIEWmk934fgrjenfmklsmdcoiwejnrhnvjnmroifjeiofjsdnfkdsnfkjdsnfsdfsd
#define VECTORIZER_CONFIGURATION_H_KFOIEWmk934fgrjenfmklsmdcoiwejnrhnvjnmroifjeiofjsdnfkdsnfkjdsnfsdfsd

#include <memory>
#include <string>

#include <common/basic_configuration.h>
#include <executor/os_proxy.h>

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

namespace vectorizer
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
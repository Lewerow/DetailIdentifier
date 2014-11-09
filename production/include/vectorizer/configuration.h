#ifndef VECTORIZER_CONFIGURATION_H_KFOIEWmk934fgrjenfmklsmdcoiwejnrhnvjnmroifjeiofjsdnfkdsnfkjdsnfsdfsd
#define VECTORIZER_CONFIGURATION_H_KFOIEWmk934fgrjenfmklsmdcoiwejnrhnvjnmroifjeiofjsdnfkdsnfkjdsnfsdfsd

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

namespace vectorizer
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
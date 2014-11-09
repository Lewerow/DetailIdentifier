#ifndef CONFIGURATOR_H_DMKiwajf9834jfrejfkgfdmkcmskmaodmweuf43n8g5hn4btgfjeuidjwdjsalkdkwejfnjrenvhui43v4rmf
#define CONFIGURATOR_H_DMKiwajf9834jfrejfkgfdmkcmskmaodmweuf43n8g5hn4btgfjeuidjwdjsalkdkwejfnjrenvhui43v4rmf

#include <memory>

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

namespace executor
{
	class application;

	class configurator
	{
	public:
		configurator(const boost::program_options::variables_map& application_params, logger::logger& log);
		~configurator();

		application& application() const;

	private:
		std::unique_ptr<::executor::application> app;
	};
}

#endif
#include <vectorizer/configuration.h>
#include <boost/program_options/variables_map.hpp>

namespace vectorizer
{
	class configuration::impl
	{
	public:
		std::shared_ptr<executor::os_proxy> os_proxy;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log, std::shared_ptr<executor::os_proxy> os) : basic_configuration(vars, log),
		pimpl(std::make_unique<impl>())
	{}

	configuration::~configuration()
	{}

	void configuration::set_os_proxy(std::shared_ptr<executor::os_proxy> os)
	{
		pimpl->os_proxy = os;
	}

	executor::os_proxy& configuration::os_proxy() const
	{
		return *pimpl->os_proxy;
	}
}
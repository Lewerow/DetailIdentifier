#include <vectorizer/configuration.h>
#include <boost/program_options/variables_map.hpp>

namespace vectorizer
{
	class configuration::impl
	{
	public:
		std::shared_ptr<executor::os_proxy> os_proxy;
		std::string potrace_location;
		std::string mkbitmap_location;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log, std::shared_ptr<executor::os_proxy> os) : basic_configuration(vars, log),
		pimpl(std::make_unique<impl>())
	{
		if (vars.count("potrace"))
			pimpl->potrace_location = vars.at("potrace").as<std::string>();

		if (vars.count("mkbitmap"))
			pimpl->mkbitmap_location = vars.at("mkbitmap").as<std::string>();

		set_os_proxy(os);
	}

	configuration::~configuration()
	{}

	void configuration::set_os_proxy(std::shared_ptr<executor::os_proxy> os)
	{
		pimpl->os_proxy = os;
	}

	void configuration::set_potrace_location(const std::string& location)
	{
		pimpl->potrace_location = location;
	}

	void configuration::set_mkbitmap_location(const std::string& location)
	{
		pimpl->mkbitmap_location = location;;
	}

	executor::os_proxy& configuration::os_proxy() const
	{
		return *pimpl->os_proxy;
	}

	std::string configuration::potrace_location() const
	{
		return pimpl->potrace_location;
	}

	std::string configuration::mkbitmap_location() const
	{
		return pimpl->mkbitmap_location;
	}
}
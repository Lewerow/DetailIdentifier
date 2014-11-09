#include <preprocessor/configuration.h>

#include <boost/program_options/variables_map.hpp>
#include <logger/logger.h>

namespace preprocessor
{
	class configuration::impl
	{
	public:
		std::string input_filename;
	};

	configuration::configuration(const boost::program_options::variables_map& vars, logger::logger& log) : basic_configuration(vars, log), pimpl(std::make_unique<impl>())
	{
		if (vars.count("input"))
			set_input_filename(vars.at("input").as<std::string>());
	}

	configuration::~configuration()
	{}

	void configuration::set_input_filename(const std::string& path)
	{
		pimpl->input_filename = path;
	}

	std::string configuration::input_filename() const
	{
		return pimpl->input_filename;
	}
}
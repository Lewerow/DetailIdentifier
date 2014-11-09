#include <logger/command_line_logger.h>

#include <iostream>
#include <sstream>

namespace
{
	void log(std::ostream& stream, const std::string& prefix, const std::string& line)
	{
		std::stringstream s;
		s << prefix << line << std::endl;
		stream << s.str();
	}
}

namespace logger
{
	void command_line_logger::debug_log(const std::string& s)
	{
		log(std::cout, "Debug: ", s);
	}

	void command_line_logger::info_log(const std::string& s)
	{
		log(std::cout, "Info: ", s);
	}

	void command_line_logger::warning_log(const std::string& s)
	{
		log(std::cerr, "Warning: ", s);
	}

	void command_line_logger::error_log(const std::string& s)
	{
		log(std::cerr, "ERROR: ", s);
	}
}

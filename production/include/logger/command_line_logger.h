#ifndef COMMAND_LINE_LOGGER_H_Feoiferuimdkcmsdokfjerngbrjdsmklsamdoife4wujnvfkdfnmerifu45rhgutmrefreferwfwefwefwefweffwegtrbgfhgdfghfdh
#define COMMAND_LINE_LOGGER_H_Feoiferuimdkcmsdokfjerngbrjdsmklsamdoife4wujnvfkdfnmerifu45rhgutmrefreferwfwefwefwefweffwegtrbgfhgdfghfdh

#include <logger/logger.h>

namespace logger
{
	class command_line_logger : public logger
	{
	public:
		virtual void debug_log(const std::string& s);
		virtual void info_log(const std::string& s);
		virtual void warning_log(const std::string& s);
		virtual void error_log(const std::string& s);
	};
}

#endif
#ifndef LOGGER_H_mf4ior4guimni45jgmfkldsfmcsdofme4wiofgm34uign54jngkrtgdsfklwemflk3mingtnijgkrfjeowf
#define LOGGER_H_mf4ior4guimni45jgmfkldsfmcsdofme4wiofgm34uign54jngkrtgdsfklwemflk3mingtnijgkrfjeowf

#include <string>

namespace logger
{
	class logger
	{
	public:
		virtual ~logger(){};

		virtual void debug_log(const std::string& s) = 0;
		virtual void info_log(const std::string& s) = 0;
		virtual void warning_log(const std::string& s) = 0;
		virtual void error_log(const std::string& s) = 0;
	};
}

#endif
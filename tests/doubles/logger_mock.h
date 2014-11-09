#ifndef LOGGER_MOCK_KIEOWFMiuof43fjrkfnvjksdmwjdf934uifgirjgnklsjrfio43hjfiu45gjkerjfdlwjedoerfj
#define LOGGER_MOCK_KIEOWFMiuof43fjrkfnvjksdmwjdf934uifgirjgnklsjrfio43hjfiu45gjkerjfdlwjedoerfj

#include <turtle/mock.hpp>

#include <logger/logger.h>

namespace mocks
{
	MOCK_BASE_CLASS(logger, ::logger::logger)
	{
		MOCK_METHOD_EXT(debug_log, 1, void(const std::string&), debug_log);
		MOCK_METHOD_EXT(info_log, 1, void(const std::string&), info_log);
		MOCK_METHOD_EXT(warning_log, 1, void(const std::string&), warning_log);
		MOCK_METHOD_EXT(error_log, 1, void(const std::string&), error_log);
	};
}

#endif
#ifndef OS_PROXY_MOCK_JIFje9w84j3fh483hfrehfjsdknfjsdxiuqwduehf8743gh8y5gh8fhuiwedjwsdksanxkjsanckjwenfiu34hufhj43f
#define OS_PROXY_MOCK_JIFje9w84j3fh483hfrehfjsdknfjsdxiuqwduehf8743gh8y5gh8fhuiwedjwsdksanxkjsanckjwenfiu34hufhj43f

#include <turtle/mock.hpp>
#include <executor/os_proxy.h>

namespace mocks
{
	MOCK_BASE_CLASS(os_proxy, executor::os_proxy)
	{
		MOCK_METHOD_EXT(call, 2, void(const std::string&, const std::vector<std::string>&), call);
		MOCK_METHOD_EXT(get_current_working_directory, 0, std::string(), get_current_working_directory);
		MOCK_METHOD_EXT(copy, 2, void(const std::string&, const std::string&), copy);
		MOCK_METHOD_EXT(load_file, 1, std::string(const std::string&), load_file);
		MOCK_METHOD_EXT(save_file, 2, void(const std::string&, const std::string&), save_file);
	};
}

#endif
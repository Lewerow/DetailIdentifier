#ifndef OS_PROXY_H_mnfiowef3489feksdmkjwqndu238f43iokfnedsldwlqkdmnwfuiergh34f4kfmdfc
#define OS_PROXY_H_mnfiowef3489feksdmkjwqndu238f43iokfnedsldwlqkdmnwfuiergh34f4kfmdfc

#include <string>
#include <vector>

namespace executor
{
	class os_proxy
	{
	public:
		virtual int call(const std::string& application, const std::vector<std::string>& arguments);
		virtual std::string get_current_working_directory();
	};
}

#endif
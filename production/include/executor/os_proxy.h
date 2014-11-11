#ifndef OS_PROXY_H_mnfiowef3489feksdmkjwqndu238f43iokfnedsldwlqkdmnwfuiergh34f4kfmdfc
#define OS_PROXY_H_mnfiowef3489feksdmkjwqndu238f43iokfnedsldwlqkdmnwfuiergh34f4kfmdfc

#include <string>
#include <vector>

namespace executor
{
	class os_proxy
	{
	public:
		virtual void call(const std::string& application, const std::vector<std::string>& arguments);
		virtual std::string get_current_working_directory();
		virtual void copy(const std::string& source, const std::string& destination);

		virtual std::string load_file(const std::string& path);
	};
}

#endif
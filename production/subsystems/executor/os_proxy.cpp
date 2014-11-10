#include <executor/os_proxy.h>
#include <executor/exceptions.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


namespace executor
{
	void os_proxy::call(const std::string& application, const std::vector<std::string>& arguments)
	{
		std::string invocation = application + " " +boost::join(arguments, " ");
		if (std::system(invocation.c_str()) != EXIT_SUCCESS)
			throw system_call_failure(invocation);
	}

	std::string os_proxy::get_current_working_directory()
	{
		return boost::filesystem::current_path().string();
	}

	void os_proxy::copy(const std::string& source, const std::string& destination)
	{
		boost::filesystem::copy(source, destination);
	}
}
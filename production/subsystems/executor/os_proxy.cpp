#include <executor/os_proxy.h>
#include <executor/exceptions.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>

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

	void os_proxy::save_file(const std::string& name, const std::string& content)
	{
		std::ofstream t(name);
		t << content;
	}

	std::string os_proxy::load_file(const std::string& path)
	{
		std::ifstream t(path);
		t.seekg(0, std::ios::end);
		std::size_t size = static_cast<std::size_t>(t.tellg());
		std::string buffer(size, ' ');
		t.seekg(0);
		t.read(&buffer[0], size);
		return buffer;
	}
}
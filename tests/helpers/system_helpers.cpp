#include <helpers/system_helpers.h>

#include <fstream>
#include <cassert>

namespace helpers
{
	std::string read_whole_file(const std::string& path)
	{
		std::ifstream str(path);

		str.seekg(0, std::ios::end);
		std::size_t size = str.tellg();
		
		std::string output(size, '!');
		str.seekg(0);
		str.read(&output[0], size);
		return output;
	}
}
#include <executor/exceptions.h>

namespace executor
{
	system_call_failure::system_call_failure(const std::string& call) : std::runtime_error(("System call failed: " + call).c_str())
	{}
}
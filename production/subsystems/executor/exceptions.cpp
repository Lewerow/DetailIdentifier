#include <executor/exceptions.h>

namespace executor
{
	system_call_failure::system_call_failure(const std::string& call) : runtime_error(("System call failed: " + call).c_str())
	{}
}

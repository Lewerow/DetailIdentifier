#ifndef EXECUTOR_EXCEPTIONS_H_DWmqiuodj23984fj4n3fjdskmdkqoiwemfu84rhntf89234ur9832ijeowjdksalnfreifgewj
#define EXECUTOR_EXCEPTIONS_H_DWmqiuodj23984fj4n3fjdskmdkqoiwemfu84rhntf89234ur9832ijeowjdksalnfreifgewj

#include <exception>
#include <string>

namespace executor
{
	class system_call_failure : public std::runtime_error
	{
	public:
		system_call_failure(const std::string& call);
	};

}

#endif
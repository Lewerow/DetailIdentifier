#ifndef INTERPRETER_MOCK_d332r554tgrfgvdfds4giujfwdffdfewfewfewfewfwefewfewhfg843f943jfi43jhfih43ifuh43fi843
#define INTERPRETER_MOCK_d332r554tgrfgvdfds4giujfwdffdfewfewfewfewfwefewfewhfg843f943jfi43jhfih43ifuh43fi843

#include <turtle/mock.hpp>

#include <interpreter/interpreter.h>

namespace mocks
{
	MOCK_BASE_CLASS(interpreter, ::interpreter::interpreter)
	{
		MOCK_METHOD_EXT(generate_interpretation, 1, std::unique_ptr<::interpreter::output>(std::unique_ptr<::interpreter::input>), generate_interpretation);
	};
}

#endif
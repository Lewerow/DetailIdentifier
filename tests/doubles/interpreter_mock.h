#ifndef INTERPRETER_MOCK_d332r554tgrfgvdfds4giujfwdffdfewfewfewfewfwefewfewhfg843f943jfi43jhfih43ifuh43fi843
#define INTERPRETER_MOCK_d332r554tgrfgvdfds4giujfwdffdfewfewfewfewfwefewfewhfg843f943jfi43jhfih43ifuh43fi843

#include <turtle/mock.hpp>

#include <doubles/logger_mock.h>
#include <doubles/os_proxy_mock.h>

#include <interpreter/interpreter.h>
#include <interpreter/configuration.h>

#include <helpers/simple_configuration.h>

namespace mocks
{
	MOCK_BASE_CLASS(interpreter, ::interpreter::interpreter)
	{
		interpreter(std::shared_ptr<os_proxy> os) : ::interpreter::interpreter(std::make_shared<::interpreter::configuration>(helpers::get_minimal_complete_configuration(), log, os))
		{}

		MOCK_METHOD_EXT(generate_interpretation, 1, std::unique_ptr<::interpreter::output>(std::unique_ptr<::interpreter::input>), generate_interpretation);

		mocks::logger log;
	};
}

#endif
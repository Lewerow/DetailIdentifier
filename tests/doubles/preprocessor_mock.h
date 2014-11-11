#ifndef PREPROCESSOR_MOCK_fi4389ju54giujfwsjdoiwejd9iuewjhf8i43hfg843f943jfi43jhfih43ifuh43fi843
#define PREPROCESSOR_MOCK_fi4389ju54giujfwsjdoiwejd9iuewjhf8i43hfg843f943jfi43jhfih43ifuh43fi843

#include <turtle/mock.hpp>

#include <doubles/logger_mock.h>

#include <preprocessor/preprocessor.h>
#include <preprocessor/configuration.h>

#include <helpers/simple_configuration.h>

namespace mocks
{
	MOCK_BASE_CLASS(preprocessor, ::preprocessor::preprocessor)
	{
		preprocessor() : ::preprocessor::preprocessor(std::make_shared<::preprocessor::configuration>(helpers::get_minimal_complete_configuration(), log))
		{}

		MOCK_METHOD_EXT(preprocess, 1, std::unique_ptr<::preprocessor::output>(std::unique_ptr<::preprocessor::input>), preprocess);

		mocks::logger log;
	};
}

#endif
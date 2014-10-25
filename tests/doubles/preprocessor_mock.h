#ifndef PREPROCESSOR_MOCK_fi4389ju54giujfwsjdoiwejd9iuewjhf8i43hfg843f943jfi43jhfih43ifuh43fi843
#define PREPROCESSOR_MOCK_fi4389ju54giujfwsjdoiwejd9iuewjhf8i43hfg843f943jfi43jhfih43ifuh43fi843

#include <turtle/mock.hpp>

#include <preprocessor/preprocessor.h>

namespace mocks
{
	MOCK_BASE_CLASS(preprocessor, ::preprocessor::preprocessor)
	{
		MOCK_METHOD_EXT(preprocess, 1, std::unique_ptr<::preprocessor::output>(std::unique_ptr<::preprocessor::input>), preprocess);
	};
}

#endif
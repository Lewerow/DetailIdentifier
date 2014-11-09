#ifndef VECTORIZER_MOCK_fi4389fdafdfjerfuergt5rg4trggitrgrtgtrgujfwsjdoiwejd9iuewjhf8i43hfg843f943jfi43jhfih43ifuh43fi843
#define VECTORIZER_MOCK_fi4389fdafdfjerfuergt5rg4trggitrgrtgtrgujfwsjdoiwejd9iuewjhf8i43hfg843f943jfi43jhfih43ifuh43fi843

#include <turtle/mock.hpp>

#include <doubles/logger_mock.h>

#include <vectorizer/vectorizer.h>
#include <vectorizer/configuration.h>

#include <helpers/simple_configuration.h>

namespace mocks
{
	MOCK_BASE_CLASS(vectorizer, ::vectorizer::vectorizer)
	{
		vectorizer() : ::vectorizer::vectorizer(::vectorizer::configuration(helpers::get_minimal_complete_configuration(), log))
		{}

		MOCK_METHOD_EXT(vectorize, 1, std::unique_ptr<::vectorizer::output>(std::unique_ptr<::vectorizer::input>), vectorize);

		mocks::logger log;
	};
}

#endif
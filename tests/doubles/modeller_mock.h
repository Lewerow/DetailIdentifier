#ifndef MODELLER_MOCK_Moiewjf34gfu4hnfgrjkdsUDFYFHWEUHSHFDSNKFKDSJFnhks
#define MODELLER_MOCK_Moiewjf34gfu4hnfgrjkdsUDFYFHWEUHSHFDSNKFKDSJFnhks

#include <turtle/mock.hpp>

#include <doubles/logger_mock.h>

#include <modeller/modeller.h>
#include <modeller/configuration.h>

#include <helpers/simple_configuration.h>

namespace mocks
{
	MOCK_BASE_CLASS(modeller, ::modeller::modeller)
	{
		modeller() : ::modeller::modeller(::modeller::configuration(helpers::get_minimal_complete_configuration(), log))
		{}

		MOCK_METHOD_EXT(generate_model, 1, std::unique_ptr<::modeller::output>(std::unique_ptr<::modeller::input>), generate_model);

		mocks::logger log;
	};
}

#endif
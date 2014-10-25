#ifndef MODELLER_MOCK_Moiewjf34gfu4hnfgrjkdsUDFYFHWEUHSHFDSNKFKDSJFnhks
#define MODELLER_MOCK_Moiewjf34gfu4hnfgrjkdsUDFYFHWEUHSHFDSNKFKDSJFnhks

#include <turtle/mock.hpp>

#include <modeller/modeller.h>

namespace mocks
{
	MOCK_BASE_CLASS(modeller, ::modeller::modeller)
	{
		MOCK_METHOD_EXT(generate_model, 1, std::unique_ptr<::modeller::output>(std::unique_ptr<::modeller::input>), generate_model);
	};
}

#endif
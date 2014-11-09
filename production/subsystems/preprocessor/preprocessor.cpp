#include <preprocessor/preprocessor.h>

namespace preprocessor
{
	preprocessor::preprocessor(const configuration&)
	{}

	preprocessor::~preprocessor()
	{}

	std::unique_ptr<output> preprocessor::preprocess(std::unique_ptr<input> in)
	{
		return std::unique_ptr<output>();
	}
}
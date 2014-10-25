#include <executor/processor.h>
#include <preprocessor/preprocessor.h>

namespace executor
{
	processor::processor(std::unique_ptr<preprocessor::preprocessor> prep) : preprocessor(std::move(prep))
	{}

	void processor::process(std::unique_ptr<preprocessor::input> input)
	{}
}
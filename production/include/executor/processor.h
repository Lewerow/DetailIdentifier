#ifndef PROCESSOR_H_dm3i20fj9843iuefnkjrenfkjenmfirfniu34jf9j34iofjewofjef
#define PROCESSOR_H_dm3i20fj9843iuefnkjrenfkjenmfirfniu34jf9j34iofjewofjef

#include <memory>

#include <common/preprocessor_interface_types.h>

namespace preprocessor
{
	class preprocessor;
}

namespace executor
{
	class processor
	{
	public:
		processor(std::unique_ptr<preprocessor::preprocessor>);

		void process(std::unique_ptr<preprocessor::input> input);
	private:
		std::unique_ptr<preprocessor::preprocessor> preprocessor;
	};
}

#endif
#ifndef PROCESSOR_H_dm3i20fj9843iuefnkjrenfkjenmfirfniu34jf9j34iofjewofjef
#define PROCESSOR_H_dm3i20fj9843iuefnkjrenfkjenmfirfniu34jf9j34iofjewofjef

#include <memory>

#include <common/preprocessor_interface_types.h>
#include <common/modeller_interface_types.h>

namespace preprocessor
{
	class preprocessor;
}

namespace vectorizer
{
	class vectorizer;
}

namespace interpreter
{
	class interpreter;
}

namespace modeller
{
	class modeller;
}

namespace executor
{
	class processor
	{
	public:
		processor(std::unique_ptr<preprocessor::preprocessor>, std::unique_ptr<vectorizer::vectorizer>, std::unique_ptr<interpreter::interpreter>, std::unique_ptr<modeller::modeller>);

		std::unique_ptr<modeller::output> process(std::unique_ptr<preprocessor::input> input);
	private:
		std::unique_ptr<preprocessor::preprocessor> preprocessor;
		std::unique_ptr<vectorizer::vectorizer> vectorizer;
		std::unique_ptr<interpreter::interpreter> interpreter;
		std::unique_ptr<modeller::modeller> modeller;
	};
}

#endif
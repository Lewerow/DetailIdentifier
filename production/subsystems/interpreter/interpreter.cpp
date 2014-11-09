#include <interpreter/interpreter.h>

namespace interpreter
{
	interpreter::interpreter(const configuration&)
	{}

	interpreter::~interpreter()
	{}

	std::unique_ptr<output> interpreter::generate_interpretation(std::unique_ptr<input> in)
	{
		return std::unique_ptr<output>();
	}
}
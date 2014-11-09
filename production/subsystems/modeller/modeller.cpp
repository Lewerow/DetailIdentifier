#include <modeller/modeller.h>

namespace modeller
{
	modeller::modeller(const configuration&)
	{}

	modeller::~modeller()
	{}

	std::unique_ptr<output> modeller::generate_model(std::unique_ptr<input> in)
	{
		return std::unique_ptr<output>();
	}
}
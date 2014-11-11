#include <modeller/modeller.h>

namespace modeller
{
	modeller::modeller(std::shared_ptr<configuration>)
	{}

	modeller::~modeller()
	{}

	std::unique_ptr<output> modeller::generate_model(std::unique_ptr<input> in)
	{
		return std::make_unique<output>();
	}
}
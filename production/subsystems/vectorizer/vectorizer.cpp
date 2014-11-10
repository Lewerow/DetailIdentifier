#include <vectorizer/vectorizer.h>

namespace vectorizer
{
	class vectorizer::impl
	{};

	vectorizer::vectorizer(const configuration& config) : pimpl(std::make_unique<impl>())
	{
	
	}
	
	vectorizer::~vectorizer() 
	{}

	std::unique_ptr<output> vectorizer::vectorize(std::unique_ptr<input> in)
	{

		return std::unique_ptr<output>();
	}
}
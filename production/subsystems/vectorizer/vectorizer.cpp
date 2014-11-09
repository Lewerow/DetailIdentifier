#include <vectorizer/vectorizer.h>

namespace vectorizer
{
	vectorizer::vectorizer(const configuration&)
	{}
	
	vectorizer::~vectorizer() 
	{}

	std::unique_ptr<output> vectorizer::vectorize(std::unique_ptr<input> in)
	{
		return std::unique_ptr<output>();
	}
}
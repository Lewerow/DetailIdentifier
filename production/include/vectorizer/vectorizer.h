#ifndef VECTORIZER_H_ewiodf93428jfirefewjxsaxerfreferfresaxqweijfowqdqwdqwdqwdwqdiwef
#define VECTORIZER_H_ewiodf93428jfirefewjxsaxerfreferfresaxqweijfowqdqwdqwdqwdwqdiwef

#include <common/vectorizer_interface_types.h>

namespace vectorizer
{
	class vectorizer
	{
	public:
		virtual ~vectorizer() {}

		virtual std::unique_ptr<output> vectorize(std::unique_ptr<input> in) = 0;
	};
}

#endif
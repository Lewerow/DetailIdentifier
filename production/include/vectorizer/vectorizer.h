#ifndef VECTORIZER_H_ewiodf93428jfirefewjxsaxerfreferfresaxqweijfowqdqwdqwdqwdwqdiwef
#define VECTORIZER_H_ewiodf93428jfirefewjxsaxerfreferfresaxqweijfowqdqwdqwdqwdwqdiwef

#include <memory>
#include <common/vectorizer_interface_types.h>

namespace vectorizer
{
	class configuration;
	class vectorizer
	{
		class impl;
	public:
		vectorizer(const configuration&);
		virtual ~vectorizer();

		virtual std::unique_ptr<output> vectorize(std::unique_ptr<input> in);

	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
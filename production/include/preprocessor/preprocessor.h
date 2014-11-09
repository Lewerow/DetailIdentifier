#ifndef PREPROCESSOR_H_ewiodf93428jfirefewjdofwejdjnewrhfreughrtghuewfjewiodjoweijfoiwef
#define PREPROCESSOR_H_ewiodf93428jfirefewjdofwejdjnewrhfreughrtghuewfjewiodjoweijfoiwef

#include <memory>
#include <common/preprocessor_interface_types.h>

namespace preprocessor
{
	class configuration;
	class preprocessor
	{
		class impl;
	public:
		preprocessor(const configuration&);
		virtual ~preprocessor();

		virtual std::unique_ptr<output> preprocess(std::unique_ptr<input> in);

	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
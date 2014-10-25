#ifndef PREPROCESSOR_H_ewiodf93428jfirefewjdofwejdjnewrhfreughrtghuewfjewiodjoweijfoiwef
#define PREPROCESSOR_H_ewiodf93428jfirefewjdofwejdjnewrhfreughrtghuewfjewiodjoweijfoiwef

#include <common/preprocessor_interface_types.h>

namespace preprocessor
{
	class preprocessor
	{
	public:
		virtual ~preprocessor() {}

		virtual std::unique_ptr<output> preprocess(std::unique_ptr<input> in) = 0;
	};
}

#endif
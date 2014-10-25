#ifndef MODELLER_H_Dkm023jf8943jgfmdlkcmsodmeiwufnrigvfkdlsokJHNJIOJFneriff
#define MODELLER_H_Dkm023jf8943jgfmdlkcmsodmeiwufnrigvfkdlsokJHNJIOJFneriff

#include <common/modeller_interface_types.h>

namespace modeller
{
	class modeller
	{
	public:
		virtual ~modeller() {}

		virtual std::unique_ptr<output> generate_model(std::unique_ptr<input> in) = 0;
	};
}

#endif
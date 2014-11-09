#ifndef MODELLER_H_Dkm023jf8943jgfmdlkcmsodmeiwufnrigvfkdlsokJHNJIOJFneriff
#define MODELLER_H_Dkm023jf8943jgfmdlkcmsodmeiwufnrigvfkdlsokJHNJIOJFneriff

#include <memory>
#include <common/modeller_interface_types.h>

namespace modeller
{
	class configuration;
	class modeller
	{
	public:
		modeller(const configuration&);
		virtual ~modeller();

		virtual std::unique_ptr<output> generate_model(std::unique_ptr<input> in);
	};
}

#endif
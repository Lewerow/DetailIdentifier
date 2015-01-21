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
		modeller(std::shared_ptr<configuration>);
		virtual ~modeller();

		void draw_cast_lines(const input& in);
		virtual std::unique_ptr<output> generate_model(std::unique_ptr<input> in);

	private:
		std::shared_ptr<configuration> cfg;
	};
}

#endif
#ifndef INTERPRETER_H_doewjf9843fjrekjmcdksmcswmdewmifunrgiutrgorefjhnrtgijreoifgjreiojfreg
#define INTERPRETER_H_doewjf9843fjrekjmcdksmcswmdewmifunrgiutrgorefjhnrtgijreoifgjreiojfreg

#include <memory>
#include <common/interpreter_interface_types.h>

namespace interpreter
{
	class configuration;
	class interpreter
	{
		class impl;
	public:
		interpreter(std::shared_ptr<configuration>);
		virtual ~interpreter();

		virtual std::unique_ptr<output> generate_interpretation(std::unique_ptr<input> in);

	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
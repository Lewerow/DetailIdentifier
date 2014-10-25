#ifndef INTERPRETER_H_doewjf9843fjrekjmcdksmcswmdewmifunrgiutrgorefjhnrtgijreoifgjreiojfreg
#define INTERPRETER_H_doewjf9843fjrekjmcdksmcswmdewmifunrgiutrgorefjhnrtgijreoifgjreiojfreg

#include <common/interpreter_interface_types.h>

namespace interpreter
{
	class interpreter
	{
	public:
		virtual ~interpreter() {}

		virtual std::unique_ptr<output> generate_interpretation(std::unique_ptr<input> in) = 0;
	};
}

#endif
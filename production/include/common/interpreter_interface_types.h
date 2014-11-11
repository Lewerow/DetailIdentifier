#ifndef INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf
#define INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf

#include <common/vectorizer_interface_types.h>

namespace interpreter
{
	typedef vectorizer::output input;

	class output
	{
	public:
		std::string svg_filename;
		std::string dxf_filename;
	};
}

#endif
#ifndef INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf
#define INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf

#include <memory>
#include <map>
#include <common/vectorizer_interface_types.h>

#include <interpreter/svg/document.h>

namespace interpreter
{
	typedef vectorizer::output input;

	class output
	{
	public:
		std::string svg_filename;
		std::string dxf_filename;

		std::map<std::string, std::unique_ptr<svg::document> > layers;
	};
}

#endif
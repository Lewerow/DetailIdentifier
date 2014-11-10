#ifndef VECTORIZER_INTERFACE_TYPES_H_mf3ifj4389fj4fwfwefwefwefjioefasfasfafwfwejflsakmdkjsfsdfsdfafanfiknejdwejdoijweoifdj23498f54jgfrekjnkf
#define VECTORIZER_INTERFACE_TYPES_H_mf3ifj4389fj4fwfwefwefwefjioefasfasfafwfwejflsakmdkjsfsdfsdfafanfiknejdwejdoijweoifdj23498f54jgfrekjnkf

#include <common/preprocessor_interface_types.h>
#include <string>

namespace vectorizer
{
	typedef preprocessor::output input;

	class output
	{
	public:
		std::string pgm_input_filename;
		std::string svg_filename;
		std::string dxf_filename;
	};
}

#endif
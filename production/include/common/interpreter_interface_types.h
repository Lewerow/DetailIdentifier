#ifndef INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf
#define INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf

#include <memory>
#include <map>
#include <common/vectorizer_interface_types.h>

#include <interpreter/svg/document.h>
#include <interpreter/projection.h>

#include <opencv2/core/core.hpp>

namespace interpreter
{
	typedef vectorizer::output input;

	class output
	{
	public:
		cv::Mat skeleton_image;

		std::vector<std::vector<projection_direction> > projection_directions;
		std::size_t main_projection;

		std::vector<projection> projections;
	};
}

#endif
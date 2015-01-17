#ifndef INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf
#define INTERPRETER_INTERFACE_TYPES_H_mf3ifj4389fj4fjioejflsakmdkjsnfikdupaduapejdwejdoijweoifdj23498f54jgfrekjnkf

#include <memory>
#include <map>
#include <common/vectorizer_interface_types.h>

#include <interpreter/svg/document.h>

#include <opencv2/core/core.hpp>

namespace interpreter
{
	typedef vectorizer::output input;

	class output
	{
	public:
		cv::Mat skeleton_image;

		std::vector<cv::Point2i> vertices;
		std::vector<std::pair<std::size_t, std::size_t> > edges;
	};
}

#endif
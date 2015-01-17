#ifndef VECTORIZER_INTERFACE_TYPES_H_mf3ifj4389fj4fwfwefwefwefjioefasfasfafwfwejflsakmdkjsfsdfsdfafanfiknejdwejdoijweoifdj23498f54jgfrekjnkf
#define VECTORIZER_INTERFACE_TYPES_H_mf3ifj4389fj4fwfwefwefwefjioefasfasfafwfwejflsakmdkjsfsdfsdfafanfiknejdwejdoijweoifdj23498f54jgfrekjnkf

#include <common/preprocessor_interface_types.h>
#include <string>

#include <opencv2/core/core.hpp>

namespace vectorizer
{
	typedef preprocessor::output input;

	class output
	{
	public:
		cv::Mat skeleton_image;

		std::vector<cv::Point2i> vertices;
		std::vector<std::pair<std::size_t, std::size_t> > edges;
	};
}

#endif
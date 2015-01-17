#ifndef CAST_JSIOJFArj3489feiuowjfksanhfuit43wfgu4hwefkjewfwe
#define CAST_JSIOJFArj3489feiuowjfksanhfuit43wfgu4hwefkjewfwe

#include <vector>
#include <utility>

#include <opencv2/core/core.hpp>

namespace interpreter
{
	struct coordinate_system
	{
		cv::Point3i get_location_of(cv::Point2i p);

		cv::Point2i zero_point_image_location;
		cv::Point3i zero_point_offset;
	};

	class projection
	{
	public:
		std::vector<cv::Point2i> vertices;
		std::vector<std::pair<cv::Point2i, cv::Point2i> > edges;

		cv::RotatedRect bounding_box;
		coordinate_system s;
	};
}

#endif
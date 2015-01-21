#ifndef CAST_JSIOJFArj3489feiuowjfksanhfuit43wfgu4hwefkjewfwe
#define CAST_JSIOJFArj3489feiuowjfksanhfuit43wfgu4hwefkjewfwe

#include <vector>
#include <utility>

#include <opencv2/core/core.hpp>

namespace interpreter
{

	struct projection_direction
	{
		double alpha;
		double p;

		double get_distance_to(cv::Point2i p);
	};

	struct coordinate_system
	{
		cv::Point3i get_location_of(cv::Point2i p);
		cv::Point2i zero_point_image_location;
		
		bool has_x;
		bool has_y;
		bool has_z;
		
		projection_direction x_line;
		projection_direction y_line;
		projection_direction z_line;
	};

	class projection
	{
	public:
		std::vector<cv::Point2i> vertices;
		std::vector<std::pair<cv::Point2i, cv::Point2i> > edges;
		std::vector<std::pair<std::size_t, std::size_t> > cedges;

		cv::RotatedRect bounding_box;
		coordinate_system s;
	};
}

#endif
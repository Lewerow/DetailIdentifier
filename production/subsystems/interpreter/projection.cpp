#include <interpreter/projection.h>

namespace interpreter
{
	double projection_direction::get_distance_to(cv::Point2i pt)
	{
		return std::abs(std::cos(alpha) * pt.x + std::sin(alpha) * pt.y - p);
	}

	cv::Point3i coordinate_system::get_location_of(cv::Point2i p)
	{
		double x = 0;
		double y = 0;
		double z = 0;

		if (has_x)
		{
			double x0 = x_line.get_distance_to(zero_point_image_location);
			x = x_line.get_distance_to(p) - x0;
		}

		if (has_y)
		{
			double y0 = y_line.get_distance_to(zero_point_image_location);
			y = y_line.get_distance_to(p) - y0;
		}

		if (has_z)
		{
			double z0 = z_line.get_distance_to(zero_point_image_location);
			z = z_line.get_distance_to(p) - z0;
		}
		
		return cv::Point3i{ static_cast<int>(x), static_cast<int>(y), static_cast<int>(z) };
	}
}
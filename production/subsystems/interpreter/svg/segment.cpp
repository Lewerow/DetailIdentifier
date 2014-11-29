#include <interpreter/svg/segment.h>
#include <interpreter/svg/point.h>

#include <cmath>

namespace svg
{
	segment::segment(point& start_, point& end_) : start(start_), end(end_)
	{}

	point* segment::start_point() const
	{
		return &start;
	}

	point* segment::end_point() const
	{
		return &end;
	}

	double segment::lenght() const
	{
		double x_diff = start.loc().x - end.loc().x;
		double y_diff = start.loc().y - end.loc().y;

		return std::sqrt(x_diff*x_diff + y_diff*y_diff);
	}

	std::shared_ptr<edge> segment::reverted() const
	{
		return std::make_shared<segment>(end, start);
	}
}

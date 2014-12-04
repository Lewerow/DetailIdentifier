#include <interpreter/svg/location.h>

#include <cmath>

namespace svg
{

	location::location() : x(0), y(0)
	{}

	location::location(coordinate x_, coordinate y_) : x(x_), y(y_)
	{}

	double cartesian_distance(svg::location l1, svg::location l2)
	{
		double x_diff = l1.x - l2.x;
		double y_diff = l1.y - l2.y;

		return std::sqrt(x_diff*x_diff + y_diff*y_diff);
	}

    bool operator==(const svg::location& lhs, const svg::location& rhs)
    {
	    return lhs.x == rhs.x && lhs.y == rhs.y;
    }
	bool operator!=(const svg::location& lhs, const svg::location& rhs)
	{
		return ! (lhs == rhs);
	}
}
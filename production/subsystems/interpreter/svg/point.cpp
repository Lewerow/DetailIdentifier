#include <interpreter/svg/point.h>

namespace svg
{
	point::point(svg::location loc) : location_(loc)
	{}

	const location & point::location() const
	{
		return location_;
	}
}

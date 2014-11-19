#include <interpreter/svg/document.h>

namespace svg
{
	document::document(coordinate_interval width, coordinate_interval height) : dimensions(std::make_pair(width, height))
	{}

	std::map<location, point>& document::points()
	{
		return points_;
	}

	const coordinate_interval document::width() const
	{
		return dimensions.first;
	}

	const coordinate_interval document::height() const
	{
		return dimensions.second;
	}

	void document::add_point(point p)
	{
		points_.insert(std::make_pair(p.location(), std::move(p)));
	}
}
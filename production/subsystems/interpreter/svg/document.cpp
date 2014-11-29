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
		points_.insert(std::make_pair(p.loc(), std::move(p)));
	}

	bool document::contains_point_at(svg::location loc)
	{
		return points_.count(loc) == 1;
	}

	point& document::at(svg::location loc)
	{
		return points_.at(loc);
	}
}

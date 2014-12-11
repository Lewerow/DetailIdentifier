#include <interpreter/svg/segment.h>
#include <interpreter/svg/point.h>

#include <cmath>
#include <pugixml.hpp>

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

	void segment::dump(pugi::xml_node* my_node) const
	{
		my_node->set_name("line");
		auto x1 = my_node->append_attribute("x1");
		x1.set_value(start.loc().x);
		auto y1 = my_node->append_attribute("y1");
		y1.set_value(start.loc().y);
		auto x2 = my_node->append_attribute("x2");
		x2.set_value(end.loc().x);
		auto y2 = my_node->append_attribute("y2");
		y2.set_value(end.loc().y);
		auto style = my_node->append_attribute("style");
		style.set_value("stroke:rgb(255,0,0);stroke-width:5");
	}
}

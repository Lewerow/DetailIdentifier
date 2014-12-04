#include <interpreter/svg/document.h>
#include <interpreter/svg/segment.h>
#include <pugixml.hpp>

#include <sstream>

namespace svg
{
	document::document(coordinate_interval width, coordinate_interval height) : dimensions(std::make_pair(width, height))
	{}

	std::map<location, point>& document::points()
	{
		return points_;
	}

	std::vector<std::shared_ptr<svg::edge> >& document::edges()
	{
		return edges_;
	}

	void document::add_segment(svg::location l1, svg::location l2)
	{
		if (points_.count(l1) == 0)
			points_.insert(std::make_pair(l1, svg::point(l1)));
		if (points_.count(l2) == 0)
			points_.insert(std::make_pair(l2, svg::point(l2)));

		auto edge = std::make_shared<svg::segment>(points_.at(l1), points_.at(l2));
		edges_.push_back(edge);
		points_.at(l1).add_edge(edge);
		points_.at(l2).add_edge(edge->reverted());

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

	std::string document::dump()
	{
		pugi::xml_document doc;
		auto root = doc.append_child("svg");
		auto wid = root.append_attribute("width");
		wid.set_value(width());
		auto hei = root.append_attribute("height");
		hei.set_value(height());
		
		for (auto e : edges())
			e->dump(&root.append_child("edge"));//name will be changed by dump

		std::stringstream s;
		doc.save(s);

		return s.str();
	}
}

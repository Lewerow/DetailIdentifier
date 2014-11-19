#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/segment.h>

#include <boost/lexical_cast.hpp>

#include <pugixml.hpp>

#include <functional>

namespace
{
	void parse_segment(const pugi::xml_node& line_node, svg::document& doc)
	{
		auto x_start = std::string(line_node.attribute("x1").value());
		auto y_start = std::string(line_node.attribute("y1").value());
		auto x_end = std::string(line_node.attribute("x2").value());
		auto y_end = std::string(line_node.attribute("y2").value());

		svg::point p_start(svg::location(boost::lexical_cast<svg::coordinate_interval>(x_start), (boost::lexical_cast<svg::coordinate_interval>(y_start))));
		svg::point p_end(svg::location(boost::lexical_cast<svg::coordinate_interval>(x_end), (boost::lexical_cast<svg::coordinate_interval>(y_end))));

		doc.add_point(p_start);
		doc.add_point(p_end);

		auto edge = std::make_shared<svg::segment>(doc.points().at(p_start.location()), doc.points().at(p_end.location()));
		
		doc.points().at(p_start.location()).add_edge(edge);
		doc.points().at(p_end.location()).add_edge(edge->reverted());
	}
}

namespace svg
{
	class reader::impl
	{
	public:
		impl()
		{
			parsers.insert(std::make_pair("line", parse_segment));
		}

		std::map < std::string, std::function<void(const pugi::xml_node&, document&)> > parsers;
	};

	reader::reader() : pimpl(std::make_unique<impl>())
	{}

	reader::~reader()
	{}

	std::unique_ptr<document> reader::parse(const std::string& str)
	{
		if (str.empty())
			return nullptr;

		pugi::xml_document xml_doc;
		
		if (!xml_doc.load(str.c_str()))
			throw std::runtime_error("Cannot load given svg file");
		
		auto width = std::string(xml_doc.child("svg").attribute("width").value());
		auto height = std::string(xml_doc.child("svg").attribute("height").value());
		auto svg_doc = std::make_unique<document>(boost::lexical_cast<coordinate_interval>(width), boost::lexical_cast<coordinate_interval>(height));

		for (auto node = xml_doc.child("svg").first_child(); node; node = node.next_sibling())
			pimpl->parsers[node.name()](node, *svg_doc);

		return svg_doc;
	}
}
#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/segment.h>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <pugixml.hpp>

#include <functional>

namespace svg
{
	struct path_element
	{
		virtual bool is_path_start() 
		{
			return false;
		}

		virtual std::shared_ptr<svg::edge> make_edge(svg::point& start, svg::point& end) = 0;

		virtual svg::location endpoint() = 0;
	};

	struct path_starting_point : public path_element
	{
		bool is_path_start() 
		{ return true; }

		svg::location endpoint()
		{
			return loc;
		}

		std::shared_ptr<svg::edge> make_edge(svg::point&, svg::point&)
		{
			assert(0);
			return nullptr;
		}

		svg::location loc;
	};

	struct straight_line : public path_element
	{
		svg::location endpoint()
		{
			return endpoint_;
		}
		
		std::shared_ptr<svg::edge> make_edge(svg::point& start, svg::point& end)
		{
			return std::make_shared<svg::segment>(start, end);
		}

		svg::location endpoint_;
	};
}

namespace
{
	void parse_segment(const pugi::xml_node& line_node, svg::document& doc)
	{
		std::string x_start = line_node.attribute("x1").value();
		std::string y_start = line_node.attribute("y1").value();
		std::string x_end = line_node.attribute("x2").value();
		std::string y_end = line_node.attribute("y2").value();

		svg::point p_start(svg::location(boost::lexical_cast<svg::coordinate_interval>(x_start), (boost::lexical_cast<svg::coordinate_interval>(y_start))));
		svg::point p_end(svg::location(boost::lexical_cast<svg::coordinate_interval>(x_end), (boost::lexical_cast<svg::coordinate_interval>(y_end))));

		doc.add_point(p_start);
		doc.add_point(p_end);

		auto edge = std::make_shared<svg::segment>(doc.points().at(p_start.location()), doc.points().at(p_end.location()));
		
		doc.points().at(p_start.location()).add_edge(edge);
		doc.points().at(p_end.location()).add_edge(edge->reverted());
	}

	typedef boost::char_separator<char> svg_separator;
	typedef std::unique_ptr<svg::path_element> raw_path;
	typedef std::function<raw_path(std::string, const std::vector<raw_path>&)> path_element_creator;

	struct absolute_location
	{
		static svg::location get_starting_point(const std::vector<raw_path>&)
		{
			return svg::location(0, 0);
		}
	};

	struct relative_location
	{
		static svg::location get_starting_point(const std::vector<raw_path>& history)
		{
			if (history.empty())
				throw std::logic_error("Expected element before relative location");

			return history.back()->endpoint();
		}
	};

	std::vector<svg::coordinate> fetch_params(std::string serialized, std::size_t param_count, const std::string& symbols)
	{
		svg_separator separator((symbols + " ,;").c_str(), "", boost::drop_empty_tokens);
		boost::tokenizer<svg_separator> tokens(serialized, separator);

		std::vector<svg::coordinate> coordinates;
		for (boost::tokenizer<svg_separator>::iterator it = tokens.begin(); it != tokens.end(); ++it)
			coordinates.push_back(boost::lexical_cast<svg::coordinate>(*it));

		if (coordinates.size() != param_count)
			throw std::logic_error("Invalid number of coordinates");

		return coordinates;
	}

	template <typename location_policy>
	raw_path move_to(const std::string& serialized, const std::vector<raw_path>& history)
	{
		auto coordinates = fetch_params(serialized, 2, "Mm");
		std::unique_ptr<svg::path_starting_point> result(std::make_unique<svg::path_starting_point>());
		svg::location start_point = location_policy::get_starting_point(history);
		result->loc = svg::location(start_point.x + coordinates[0], start_point.y + coordinates[1]);
		return std::move(result);
	}
	
	template <typename location_policy>
	raw_path straight_line(std::string serialized, const std::vector<raw_path>& history)
	{
		std::vector<svg::coordinate> coordinates = fetch_params(serialized, 2, "Ll");
		std::unique_ptr<svg::straight_line> line(std::make_unique<svg::straight_line>());

		svg::location start_point = location_policy::get_starting_point(history);
		line->endpoint_.x = coordinates[0] + start_point.x;
		line->endpoint_.y = coordinates[1] + start_point.y;

		return std::move(line);
	}

	raw_path close_path(std::string, const std::vector<raw_path>& history)
	{
		if (history.size() < 2)
			throw std::logic_error("Too few elements for closing path");

		std::unique_ptr<svg::straight_line> line(std::make_unique<svg::straight_line>());
		line->endpoint_ = history.front()->endpoint();

		return std::move(line);
	}

	std::map<char, path_element_creator> get_path_parsers()
	{
		std::map<char, path_element_creator> parsers;
		parsers.insert(std::make_pair('M', move_to<absolute_location>));
		parsers.insert(std::make_pair('m', move_to<relative_location>));
    	parsers.insert(std::make_pair('L', straight_line<absolute_location>));
		parsers.insert(std::make_pair('l', straight_line<relative_location>));
		parsers.insert(std::make_pair('Z', close_path));
		parsers.insert(std::make_pair('z', close_path));

		return parsers;
	}

	bool is_next_path(const std::string& s)
	{
		return s[0] == 'm' || s[0] == 'M';
	}

	void parse_path(const pugi::xml_node& path_node, svg::document& doc)
	{
		auto path_parsers = get_path_parsers();

		std::string textual_representation = path_node.attribute("d").value();

		svg_separator separator("", "MmLlHhVvCcSsQqTtAaZz", boost::drop_empty_tokens);
		boost::tokenizer<svg_separator> tokens(textual_representation, separator);
				
		std::vector<std::string> splitted_into_tokens;
		for (boost::tokenizer<svg_separator>::iterator it = tokens.begin(); it != tokens.end(); ++it)
			splitted_into_tokens.push_back(*it);

		std::vector<std::vector<std::unique_ptr<svg::path_element> > > elements(1);
		std::size_t current_path_number = 0;
		for (std::size_t i = 0; i < splitted_into_tokens.size(); ++i)
		{
			std::string trimmed(boost::algorithm::trim_copy(splitted_into_tokens[i]));
			if (i < splitted_into_tokens.size() - 1)
				trimmed += splitted_into_tokens[++i];

			raw_path element = path_parsers[trimmed[0]](trimmed, elements[current_path_number]);
			if (element->is_path_start())
			{
				elements.push_back(std::vector<std::unique_ptr<svg::path_element> >());
				++current_path_number;
			}
			
			elements[current_path_number].push_back(std::move(element));
		}

		for (auto& single_path : elements)
		{
			svg::location previous_location;
			for (auto& elem : single_path)
			{
				if (elem->is_path_start())
					doc.add_point(svg::point(elem->endpoint()));

				else
				{
					svg::point p(elem->endpoint());
					if (doc.contains_point_at(elem->endpoint()))
						doc.at(elem->endpoint()).add_edge(elem->make_edge(doc.at(elem->endpoint()), doc.at(previous_location)));
					else
					{
						doc.add_point(p);
						doc.at(p.location()).add_edge(elem->make_edge(doc.at(elem->endpoint()), doc.at(previous_location)));
					}

					doc.at(previous_location).add_edge(elem->make_edge(doc.at(previous_location), doc.at(elem->endpoint())));
				}

				previous_location = elem->endpoint();
			}

		}	
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
			parsers.insert(std::make_pair("path", parse_path));
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
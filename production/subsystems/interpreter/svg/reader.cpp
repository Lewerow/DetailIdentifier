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
	typedef double raw_coordinate;
	typedef raw_coordinate raw_coordinate_interval;

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

		svg::point p_start(svg::location(boost::lexical_cast<svg::raw_coordinate_interval>(x_start), (boost::lexical_cast<svg::raw_coordinate_interval>(y_start))));
		svg::point p_end(svg::location(boost::lexical_cast<svg::raw_coordinate_interval>(x_end), (boost::lexical_cast<svg::raw_coordinate_interval>(y_end))));

		doc.add_point(p_start);
		doc.add_point(p_end);

		auto edge = std::make_shared<svg::segment>(doc.points().at(p_start.loc()), doc.points().at(p_end.loc()));
		doc.add_segment(p_start.loc(), p_end.loc());
	}

	typedef boost::char_separator<char> svg_separator;
	typedef std::unique_ptr<svg::path_element> raw_path;
	typedef std::function<std::vector<raw_path>(std::string, const std::vector<raw_path>&)> path_element_creator;

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
				return svg::location(0, 0);

			return history.back()->endpoint();
		}
	};

	std::vector<svg::raw_coordinate> fetch_params(const std::string& serialized, std::size_t param_count, const std::string& symbols)
	{
		svg_separator separator((symbols + " ,;").c_str(), "", boost::drop_empty_tokens);
		boost::tokenizer<svg_separator> tokens(serialized, separator);

		std::vector<svg::raw_coordinate> coordinates;
		for (boost::tokenizer<svg_separator>::iterator it = tokens.begin(); it != tokens.end(); ++it)
			coordinates.push_back(boost::lexical_cast<svg::raw_coordinate>(*it));

		if (coordinates.size() % param_count != 0 && !coordinates.empty())
			throw std::logic_error("Invalid number of coordinates");
		
		return coordinates;
	}

	template <typename location_policy>
	std::vector<raw_path> move_to(const std::string& serialized, const std::vector<raw_path>& history)
	{
		auto coordinates = fetch_params(serialized, 2, "Mm");
		std::unique_ptr<svg::path_starting_point> path_start(std::make_unique<svg::path_starting_point>());
		svg::location start_point = location_policy::get_starting_point(history);
		path_start->loc.x = coordinates[0] + start_point.x;
		path_start->loc.y = coordinates[1] + start_point.y;

        std::vector<raw_path> res;
		res.push_back(std::move(path_start));
		for (std::size_t i = 2; i < coordinates.size(); i += 2)
		{
			auto next_line = std::make_unique<svg::straight_line>();
			next_line->endpoint_.x = coordinates[i] + res.back()->endpoint().x;
			next_line->endpoint_.y = coordinates[i + 1] + res.back()->endpoint().y;
			res.push_back(std::move(next_line));
		}

		return res;
	}
	
	template <typename location_policy>
	std::vector<raw_path> straight_line(std::string serialized, const std::vector<raw_path>& history)
	{
		std::vector<svg::raw_coordinate> coordinates = fetch_params(serialized, 2, "Ll");

		svg::location start_point = location_policy::get_starting_point(history);
		std::vector<raw_path> res;

		auto start = std::make_unique<svg::path_starting_point>();
		start->loc.x = start_point.x;
		start->loc.y = start_point.y;
		
		res.push_back(std::move(start));
		for (std::size_t i = 0; i < coordinates.size(); i += 2)
		{
			auto next_line = std::make_unique<svg::straight_line>();
			next_line->endpoint_.x = coordinates[i] + res.back()->endpoint().x;
			next_line->endpoint_.y = coordinates[i + 1] + res.back()->endpoint().y;
			res.push_back(std::move(next_line));
		}

		res.erase(res.begin());
		return res;
	}

	std::vector<raw_path> close_path(std::string, const std::vector<raw_path>& history)
	{
		if (history.size() < 2)
			throw std::logic_error("Too few elements for closing path");

		std::unique_ptr<svg::straight_line> line(std::make_unique<svg::straight_line>());
		line->endpoint_ = history.front()->endpoint();

		std::vector<raw_path> res;
		res.push_back(std::move(line));
		return res;
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

			std::size_t k = 0;
			auto next_elements = path_parsers[trimmed[0]](trimmed, elements[current_path_number]);
			if (next_elements[0]->is_path_start())
			{
				elements.push_back(std::vector<std::unique_ptr<svg::path_element> >());
				++current_path_number;
				++k;
			}
			
			for(auto& elem : next_elements)
				elements[current_path_number].push_back(std::move(elem));
		}

		for (auto& single_path : elements)
		{
			svg::location previous_location;
			for (auto& elem : single_path)
			{
				if (elem->is_path_start())
					doc.add_point(svg::point(elem->endpoint()));

				else
					doc.add_segment(previous_location, elem->endpoint());

				previous_location = elem->endpoint();
			}

		}	
	}

	const char* ignored_tags[] = { "metadata",
		"sodipodi:namedview",
		"defs",
		"" };

	bool ignored(const char* node_name)
	{
		for (const char** tag = ignored_tags; (**tag) != 0; ++tag)
		{
			if (std::strcmp(node_name, *tag) == 0)
				return true;
		}

		return false;
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

		void parse_all(const pugi::xml_node& root, svg::document& svg_doc)
		{
			for (auto node = root.first_child(); node; node = node.next_sibling())
			{
				if (std::strcmp(node.name(), "svg") == 0 || std::strcmp(node.name(), "g") == 0)
					parse_all(node, svg_doc);
				else if(!ignored(node.name()))
					parsers[node.name()](node, svg_doc);
			}
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
		
		auto width = xml_doc.child("svg").attribute("width");
		auto height = xml_doc.child("svg").attribute("height");
		auto svg_doc = std::make_unique<document>(width.as_double(), height.as_double());

		pimpl->parse_all(xml_doc.document_element(), *svg_doc);

		return svg_doc;
	}
}

#ifndef DOCUMENT_H_CKMSk93284f9uf4foijewofjsalkmclksamdcsadsdasdsadqwqw
#define DOCUMENT_H_CKMSk93284f9uf4foijewofjsalkmclksamdcsadsdasdsadqwqw

#include <memory>
#include <map>

#include <interpreter/svg/point.h>

namespace svg
{
	class document
	{
	public:
		document(coordinate_interval width, coordinate_interval height);

		std::map<location, point>& points();
		std::vector<std::shared_ptr<svg::edge> >& edges();

		const coordinate_interval width() const;
		const coordinate_interval height() const;

		void add_point(point p);
		void add_segment(svg::location l1, svg::location l2);

		bool contains_point_at(svg::location loc);
		point& at(svg::location loc);

		std::string dump();
	private:
		std::map<location, point> points_;
		std::vector<std::shared_ptr<svg::edge> > edges_;
		std::pair<coordinate_interval, coordinate_interval> dimensions;
	};
}

#endif
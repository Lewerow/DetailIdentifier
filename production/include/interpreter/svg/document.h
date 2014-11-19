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

		const coordinate_interval width() const;
		const coordinate_interval height() const;

		void add_point(point p);

	private:
		std::map<location, point> points_;
		std::pair<coordinate_interval, coordinate_interval> dimensions;
	};
}

#endif
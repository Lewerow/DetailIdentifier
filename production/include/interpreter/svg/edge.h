#ifndef EDGE_H_hgfdbvcbvcbfgbfdbdfbdf3289jfuf43fj4839uifj8437fh847fndjkcnxcsdc
#define EDGE_H_hgfdbvcbvcbfgbfdbdfbdf3289jfuf43fj4839uifj8437fh847fndjkcnxcsdc

#include <memory>

namespace svg
{
	class point;
	class edge
	{
	public:
		virtual ~edge() {};

		virtual point* start_point() const = 0;
		virtual point* end_point() const = 0;
		virtual double lenght() const = 0;

		virtual std::shared_ptr<edge> reverted() const = 0;
	};
}

#endif
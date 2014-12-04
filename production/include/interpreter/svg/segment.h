#ifndef SEGMENT_H_miremf9834jf954jguijflkdsflkdsfmkdmvkfjjingioewkfdowejkdowiejfierf
#define SEGMENT_H_miremf9834jf954jguijflkdsflkdsfmkdmvkfjjingioewkfdowejkdowiejfierf

#include <interpreter/svg/edge.h>

namespace svg
{
	class segment : public edge
	{
	public:
		segment(point& start, point& end);
		segment& operator=(const segment&) = delete;

		virtual point* start_point() const;
		virtual point* end_point() const;
		virtual double lenght() const;
		virtual std::shared_ptr<edge> reverted() const;
		void dump(pugi::xml_node* my_node) const;

	private:
		point& start;
		point& end;
	};
}

#endif
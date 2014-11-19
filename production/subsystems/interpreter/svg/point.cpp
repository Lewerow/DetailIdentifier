#include <interpreter/svg/point.h>
#include <interpreter/svg/edge.h>

namespace svg
{
	point::point(svg::location loc) : location_(loc)
	{}

	point::~point()
	{}

	const location & point::location() const
	{
		return location_;
	}

	const std::vector<std::shared_ptr<edge> >& point::edges() const
	{
		return edges_;
	}

	void point::add_edge(std::shared_ptr<edge> edge)
	{
		edges_.push_back(std::move(edge));
	}

}

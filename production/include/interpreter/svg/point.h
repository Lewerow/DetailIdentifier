#ifndef POINT_H_fwe0fj98049fg943jfg9u8h4fguhefcjnsakjnsakjxsadiuwquidfhewufherkwlfjdwldfewfh
#define POINT_H_fwe0fj98049fg943jfg9u8h4fguhefcjnsakjnsakjxsadiuwquidfhewufherkwlfjdwldfewfh

#include <interpreter/svg/location.h>

#include <vector>
#include <memory>

namespace svg
{
	class edge;

	class point
	{
	public:
		point(location loc);
		~point();

		const svg::location& loc() const;
		const std::vector<std::shared_ptr<edge> >& edges() const;

		void add_edge(std::shared_ptr<edge> edge);

	private:
		svg::location location_;
		std::vector<std::shared_ptr<edge> > edges_;
	};
}

#endif

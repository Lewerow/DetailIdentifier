#include <boost/test/test_tools.hpp>

#include <interpreter/svg/location.h>
#include <interpreter/svg/edge.h>
#include <interpreter/svg/point.h>

#include <vector>
#include <memory>

namespace boost
{
	namespace test_tools
	{
		template <>
		struct print_log_value<svg::location>
		{
			void operator()(std::ostream& str, const svg::location& loc)
			{
				str << "(" << loc.x << "," << loc.y << ")";
			}
		};

		template <>
		struct print_log_value<std::vector<std::shared_ptr<svg::edge> > >
		{
			void operator()(std::ostream& str, const std::vector<std::shared_ptr<svg::edge> >& edges)
			{
				for (auto& edge : edges)
				{
					str << "e(";
					print_log_value<svg::location>()(str, edge->start_point()->loc());
					str << ", ";
					print_log_value<svg::location>()(str, edge->end_point()->loc());
					str << ", l:" << edge->lenght() << ")";

				}
			}
		};
	}
}

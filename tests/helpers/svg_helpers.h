#include <boost/test/test_tools.hpp>
#include <interpreter/svg/location.h>

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
	}

	bool operator==(const svg::location& lhs, const svg::location& rhs);
}
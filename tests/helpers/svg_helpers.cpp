#include <helpers/svg_helpers.h>

namespace boost
{
	bool operator==(const svg::location& lhs, const svg::location& rhs)
	{
		return ::operator==(lhs, rhs);
	}
}
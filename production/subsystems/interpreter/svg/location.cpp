#include <interpreter/svg/location.h>

namespace svg
{
	location::location(coordinate x_, coordinate y_) : x(x_), y(y_)
	{}
}


bool operator==(const svg::location& lhs, const svg::location& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
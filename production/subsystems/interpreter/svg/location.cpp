#include <interpreter/svg/location.h>

namespace svg
{

	location::location() : x(0), y(0)
	{}

	location::location(coordinate x_, coordinate y_) : x(x_), y(y_)
	{}
}


bool operator==(const svg::location& lhs, const svg::location& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
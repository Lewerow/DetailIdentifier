#ifndef LOCATION_JFJE0wjf8934jfgu43fsdjksjjkadiuwqdr9greiughreg2384ru309ireowijdsjdksajdlasjd
#define LOCATION_JFJE0wjf8934jfgu43fsdjksjjkadiuwqdr9greiughreg2384ru309ireowijdsjdksajdlasjd

namespace svg
{
	typedef int coordinate;
	typedef coordinate coordinate_interval;

	class location
	{
	public:
		location(coordinate x_, coordinate y_);
		location& operator=(const location&) = delete;

		const coordinate x;
		const coordinate y;
	};
}

#include <functional>
template <>
struct std::less<svg::location>
{
	bool operator()(const svg::location& lhs, const svg::location& rhs)
	{
		if (lhs.x != rhs.x)
			return lhs.x < rhs.x;

		return lhs.y < rhs.y;
	}
};

bool operator==(const svg::location& lhs, const svg::location& rhs);

#endif
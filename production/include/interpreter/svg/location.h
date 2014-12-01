#ifndef LOCATION_JFJE0wjf8934jfgu43fsdjksjjkadiuwqdr9greiughreg2384ru309ireowijdsjdksajdlasjd
#define LOCATION_JFJE0wjf8934jfgu43fsdjksjjkadiuwqdr9greiughreg2384ru309ireowijdsjdksajdlasjd

namespace svg
{
	typedef int coordinate;
	typedef coordinate coordinate_interval;

	class location
	{
	public:
		location();
		location(coordinate x_, coordinate y_);

		coordinate x;
		coordinate y;
	};

    bool operator==(const svg::location& lhs, const svg::location& rhs);

	double cartesian_distance(svg::location l1, svg::location l2);
}


#include <functional>

namespace std
{
  template <>
  struct less<svg::location>
  {
    bool operator()(const svg::location& lhs, const svg::location& rhs) const
    {
      if (lhs.x != rhs.x)
        return lhs.x < rhs.x;

      return lhs.y < rhs.y;
    }
  };
}

#endif

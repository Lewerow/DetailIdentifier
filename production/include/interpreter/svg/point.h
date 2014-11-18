#ifndef POINT_H_fwe0fj98049fg943jfg9u8h4fguhefcjnsakjnsakjxsadiuwquidfhewufherkwlfjdwldfewfh
#define POINT_H_fwe0fj98049fg943jfg9u8h4fguhefcjnsakjnsakjxsadiuwquidfhewufherkwlfjdwldfewfh

#include <interpreter/svg/location.h>

namespace svg
{
	class point
	{
	public:
		point(location loc);
		const location& location() const;

	private:
		svg::location location_;
	};
}

#endif
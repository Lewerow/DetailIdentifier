#ifndef DI_ASSERT_H_miocewm98fj34985j4ugnrfjdmcd903dk382jf4ufierjfdslkq90d823jfujrfvdmkldmc
#define DI_ASSERT_H_miocewm98fj34985j4ugnrfjdmcd903dk382jf4ufierjfdslkq90d823jfujrfvdmkldmc

#include <string>

struct assertion_failed
{
	assertion_failed(const std::string& msg) : what(msg)
	{}

	std::string what;
};

#define DI_ASSERT(x, msg) if(!(x)){throw assertion_failed(msg);}

#endif
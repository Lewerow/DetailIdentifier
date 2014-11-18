#ifndef READER_J_FEWm9fij349mngrnfkldsmdklsmodimewouif43uigfnff43fefdsfdsfdsfsf
#define READER_J_FEWm9fij349mngrnfkldsmdklsmodimewouif43uigfnff43fefdsfdsfdsfsf

#include <memory>
#include <string>

namespace svg
{
	class document;
	class reader
	{
	public:
		reader();
		~reader();
		std::unique_ptr<document> parse(const std::string& str);

	private:
		class impl;
		std::unique_ptr<impl> pimpl;
	};
}

#endif
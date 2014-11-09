#include <boost/test/auto_unit_test.hpp>

#include <executor/os_proxy.h>

BOOST_AUTO_TEST_SUITE(executor_tests)
BOOST_AUTO_TEST_CASE(os_proxy_allows_to_invoke_echo)
{
	executor::os_proxy p;
	std::string s = "--from=" + p.get_current_working_directory();
	BOOST_CHECK_EQUAL(0, p.call("echo", {"testing", "OS", "Proxy", "executing", s.c_str() }));
}

BOOST_AUTO_TEST_SUITE_END()
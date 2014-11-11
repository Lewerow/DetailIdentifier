#include <boost/test/auto_unit_test.hpp>

#include <boost/filesystem/operations.hpp>

#include <doubles/logger_mock.h>
#include <doubles/os_proxy_mock.h>

#include <helpers/simple_configuration.h>

#include <interpreter/configuration.h>
#include <interpreter/interpreter.h>

#include <helpers/svg_data.h>

namespace
{
	const std::string input_path_svg = TEST_FILES_DIRECTORY_STRING + "interpreter/in_sanity.svg";
	const std::string input_path_pgm = TEST_FILES_DIRECTORY_STRING + "interpreter/in_sanity.pgm";
	const std::string output_path = TEST_FILES_DIRECTORY_STRING + "workspace/interpreted.svg";
}

struct interpreter_fixture
{
	interpreter_fixture() : config(std::make_shared<interpreter::configuration>(boost::program_options::variables_map{}, log, std::make_shared<executor::os_proxy>()))
	{
		config->set_workspace(TEST_FILES_DIRECTORY_STRING + "workspace");

		in = std::make_unique<interpreter::input>();
		in->pgm_input_filename = input_path_pgm;
		in->svg_filename = input_path_svg;
	}

	mocks::logger log;
	std::shared_ptr<interpreter::configuration> config;
	std::unique_ptr<interpreter::input> in;
};

BOOST_FIXTURE_TEST_SUITE(interpreter_tests, interpreter_fixture)

BOOST_AUTO_TEST_CASE(basic_sanity_ut)
{
	auto os = std::make_shared<mocks::os_proxy>();
	config->set_os_proxy(os);

	interpreter::interpreter i(config);
	
	MOCK_EXPECT(os->load_file).with(input_path_svg).once().returns(helpers::empty_svg_file);
	auto out = i.generate_interpretation(std::move(in));
}
/*
BOOST_AUTO_TEST_CASE(basic_sanity_mt)
{
	BOOST_REQUIRE(boost::filesystem::exists(input_path_svg));
	BOOST_REQUIRE(boost::filesystem::exists(input_path_pgm));
	if (boost::filesystem::exists(output_path))
		boost::filesystem::remove(output_path);

	BOOST_REQUIRE(!boost::filesystem::exists(output_path));
	
	interpreter::interpreter i(config);
	auto out = i.generate_interpretation(std::move(in));

	BOOST_CHECK(boost::filesystem::exists(out->svg_filename));
}*/

BOOST_AUTO_TEST_SUITE_END()
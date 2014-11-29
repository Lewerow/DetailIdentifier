#include <boost/test/auto_unit_test.hpp>

#include <boost/filesystem/operations.hpp>

#include <doubles/logger_mock.h>
#include <doubles/os_proxy_mock.h>

#include <helpers/simple_configuration.h>

#include <interpreter/configuration.h>
#include <interpreter/interpreter.h>

#include <helpers/svg_data.h>

#include <interpreter/svg/document.h>

namespace
{
	const std::string input_svg_ut = TEST_FILES_DIRECTORY_STRING + "interpreter/in_sanity.svg";
	const std::string input_pgm_ut = TEST_FILES_DIRECTORY_STRING + "interpreter/in_sanity.pgm";
	const std::string output_path_dxf = TEST_FILES_DIRECTORY_STRING + "workspace/interpreted.dxf";
	
	const std::string input_svg_mt = TEST_FILES_DIRECTORY_STRING + "interpreter/line_interpretation/interpretation.svg";
	const std::string input_pgm_mt = TEST_FILES_DIRECTORY_STRING + "interpreter/line_interpretation/interpretation.pgm";
}

struct interpreter_fixture
{
	interpreter_fixture(const std::string& svg_path, const std::string& pgm_path) 
		: config(std::make_shared<interpreter::configuration>(boost::program_options::variables_map{}, log, std::make_shared<executor::os_proxy>()))
	{
		config->set_workspace(TEST_FILES_DIRECTORY_STRING + "workspace");

		in = std::make_unique<interpreter::input>();
		in->pgm_input_filename = pgm_path;
		in->svg_filename = svg_path;
	}

	mocks::logger log;
	std::shared_ptr<interpreter::configuration> config;
	std::unique_ptr<interpreter::input> in;
};

struct interpreter_fixture_ut : interpreter_fixture
{
	interpreter_fixture_ut() : interpreter_fixture(input_svg_ut, input_pgm_ut)
	{}
};

struct interpreter_fixture_mt : interpreter_fixture
{
	interpreter_fixture_mt() : interpreter_fixture(input_svg_mt, input_pgm_mt)
	{}
};

BOOST_AUTO_TEST_SUITE(interpreter_tests)

BOOST_FIXTURE_TEST_CASE(basic_sanity_ut, interpreter_fixture_ut)
{
	auto os = std::make_shared<mocks::os_proxy>();
	config->set_os_proxy(os);

	interpreter::interpreter i(config);

	MOCK_EXPECT(os->load_file).with(input_svg_ut).once().returns(helpers::empty_svg_file);
	MOCK_EXPECT(os->load_file).with(input_pgm_ut).once().returns(helpers::empty_pgm_file);
	MOCK_EXPECT(os->save_file).with(output_path_dxf, mock::any).once();
	auto out = i.generate_interpretation(std::move(in));
}

BOOST_FIXTURE_TEST_CASE(identifies_rectangle_as_single_line, interpreter_fixture_mt)
{
	interpreter::interpreter i(config);
	
	auto out = i.generate_interpretation(std::move(in));

	const auto& img = out->layers;

	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), img.size());
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), img.count("image"));
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(2), img.at("image")->points().size());
}

BOOST_AUTO_TEST_SUITE_END()

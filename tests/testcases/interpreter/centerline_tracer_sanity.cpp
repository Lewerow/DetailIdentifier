#include <boost/test/auto_unit_test.hpp>

#include <interpreter/centerline_tracer.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/reader.h>

#include <opencv2/highgui/highgui.hpp>

#include <helpers/svg_input_paths.h>
#include <helpers/system_helpers.h>

struct centerline_tracing_fixture
{
	centerline_tracing_fixture(const std::string& svg_in, const std::string& pgm_in)
	{
		pgm_file = cv::imread(pgm_in);
		svg_doc = svg::reader().parse(helpers::read_whole_file(svg_in));

		tracer = std::make_unique<interpreter::centerline_tracer>(*svg_doc, pgm_file);
	}

	cv::Mat pgm_file;
	std::unique_ptr<svg::document> svg_doc;
	std::unique_ptr<interpreter::centerline_tracer> tracer;
};

struct basic_tracer_fixture : centerline_tracing_fixture
{
	basic_tracer_fixture() : centerline_tracing_fixture(helpers::input_svg_basic, helpers::input_pgm_basic)
	{}
};

struct advanced_tracer_fixture : centerline_tracing_fixture
{
	advanced_tracer_fixture() : centerline_tracing_fixture(helpers::input_svg_advanced, helpers::input_pgm_advanced)
	{}
};


BOOST_AUTO_TEST_SUITE(interpreter_tests)
BOOST_AUTO_TEST_SUITE(centerline_tracing)

BOOST_FIXTURE_TEST_CASE(identifies_filled_rectangle_as_single_line, basic_tracer_fixture)
{
	auto centerlines = tracer->trace_centerlines();

//	BOOST_CHECK_EQUAL(static_cast<std::size_t>(2), centerlines->points().size());
}

BOOST_FIXTURE_TEST_CASE(identifies_not_filled_rectangle_as_four_lines, advanced_tracer_fixture)
{
	auto centerlines = tracer->trace_centerlines();

//	BOOST_CHECK_EQUAL(static_cast<std::size_t>(4), centerlines->points().size());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
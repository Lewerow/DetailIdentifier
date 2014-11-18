#include <boost/test/auto_unit_test.hpp>

#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>

struct reader_fixture
{
	svg::reader r;
};

BOOST_FIXTURE_TEST_SUITE(svg_reader_sanity, reader_fixture)
BOOST_AUTO_TEST_CASE(empty_string_is_nullptr)
{
	BOOST_CHECK(r.parse("").get() == nullptr);
}

BOOST_AUTO_TEST_CASE(empty_document_returns_document_with_no_points)
{
	auto res = r.parse("<svg width=\"100\" height=\"200\"></svg>");
	BOOST_CHECK(res->points().empty());
	BOOST_CHECK_EQUAL(100, res->width());
	BOOST_CHECK_EQUAL(200, res->height());
}

BOOST_AUTO_TEST_CASE(document_with_single_line_has_two_points_one_segment_and_one_path)
{
	auto res = r.parse("<svg width='100' height='200'><line x1='0' y1='20' x2='100' y2='200' /></svg>");
	auto points = res->points();

	BOOST_CHECK_EQUAL(static_cast<std::size_t>(2), points.size());
}


BOOST_AUTO_TEST_SUITE_END()
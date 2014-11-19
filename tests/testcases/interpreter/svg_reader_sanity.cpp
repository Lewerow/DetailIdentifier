#include <boost/test/auto_unit_test.hpp>

#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/edge.h>
#include <interpreter/svg/location.h>

#include <helpers/svg_helpers.h>

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
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), points.count(svg::location(0, 20)));
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), points.count(svg::location(100, 200)));
	
	auto edges = points.at(svg::location(0, 20)).edges();
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), edges.size());
	BOOST_CHECK_EQUAL(svg::location(100, 200), edges.at(0)->end_point()->location());
	BOOST_CHECK_EQUAL(svg::location(0, 20), points.at(svg::location(100, 200)).edges().at(0)->end_point()->location());
}

BOOST_AUTO_TEST_CASE(points_from_multiple_lines_are_shared)
{
	auto res = r.parse("<svg width='100' height='200'><line x1='0' y1='20' x2='100' y2='200' /><line x1='100' y1='200' x2='500' y2='0' /></svg>");
	auto points = res->points();

	BOOST_CHECK_EQUAL(static_cast<std::size_t>(3), points.size());
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), points.count(svg::location(0, 20)));
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), points.count(svg::location(100, 200)));
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(1), points.count(svg::location(500, 0)));

	auto edges = points.at(svg::location(100, 200)).edges();
	BOOST_REQUIRE_EQUAL(static_cast<std::size_t>(2), edges.size());

	auto first_endpoint = edges.at(0)->end_point();
	auto second_endpoint = edges.at(1)->end_point();
	BOOST_REQUIRE_NE(first_endpoint->location(), second_endpoint->location());
	BOOST_CHECK(first_endpoint->location() == svg::location(0, 20) || first_endpoint->location() == svg::location(500, 0));
	BOOST_CHECK(second_endpoint->location() == svg::location(0, 20) || second_endpoint->location() == svg::location(500, 0));
}

BOOST_AUTO_TEST_SUITE_END()
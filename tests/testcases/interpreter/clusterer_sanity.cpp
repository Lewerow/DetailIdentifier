#include <boost/test/auto_unit_test.hpp>

#include <interpreter/clusterer.h>

#include <random>

void add_cluster_near(std::vector<svg::location>& locations, svg::location where, std::size_t elems_in_cluster, double dev)
{
	std::mt19937 gen(4);
    
	std::normal_distribution<> xs(where.x, dev);
	std::normal_distribution<> ys(where.y, dev);

	for (std::size_t i = 0; i < elems_in_cluster; ++i)
		locations.push_back(svg::location(xs(gen), ys(gen)));
}

BOOST_AUTO_TEST_SUITE(interpreter_tests)
BOOST_AUTO_TEST_SUITE(clusterer_tests)

BOOST_AUTO_TEST_CASE(two_simple_clusters_are_identified)
{
	std::vector<svg::location> locations;
	add_cluster_near(locations, svg::location(500, 250), 10, 5);
	add_cluster_near(locations, svg::location(105, 110), 10, 5);

	interpreter::clusterer c(locations);

	auto clusters = c.clusterize();

	BOOST_REQUIRE_EQUAL(2, clusters.size());
	BOOST_CHECK_CLOSE_FRACTION(105.0, clusters[0].x, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(110.0, clusters[0].y, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(500.0, clusters[1].x, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(250.0, clusters[1].y, 0.1);
}

BOOST_AUTO_TEST_CASE(five_simple_clusters_are_identified)
{
	std::vector<svg::location> locations;
	add_cluster_near(locations, svg::location(5, 20), 40, 5);
	add_cluster_near(locations, svg::location(100, 478), 30, 5);
	add_cluster_near(locations, svg::location(800, 900), 50, 5);
	add_cluster_near(locations, svg::location(500, 200), 18, 5);
	add_cluster_near(locations, svg::location(90, 80), 35, 5);

	interpreter::clusterer c(locations);

	auto clusters = c.clusterize();

	BOOST_REQUIRE_EQUAL(5, clusters.size());
	BOOST_CHECK_CLOSE_FRACTION(5.0, clusters[0].x, 1);
	BOOST_CHECK_CLOSE_FRACTION(20.0, clusters[0].y, 0.4);
	BOOST_CHECK_CLOSE_FRACTION(90.0, clusters[1].x, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(80.0, clusters[1].y, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(100.0, clusters[2].x, 0.3);
	BOOST_CHECK_CLOSE_FRACTION(478.0, clusters[2].y, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(500.0, clusters[3].x, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(200.0, clusters[3].y, 0.5);
	BOOST_CHECK_CLOSE_FRACTION(800.0, clusters[4].x, 0.1);
	BOOST_CHECK_CLOSE_FRACTION(900.0, clusters[4].y, 0.1);
}



BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
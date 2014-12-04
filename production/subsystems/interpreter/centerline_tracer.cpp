#include <interpreter/centerline_tracer.h>
#include <interpreter/svg/document.h>
#include <interpreter/clusterer.h>

#include <interpreter/svg/location.h>
#include <interpreter/svg/edge.h>

#include <algorithm>

namespace
{
	std::pair<svg::location, svg::location> ordered_pair(svg::location l1, svg::location l2)
	{
		if (std::less<svg::location>()(l1, l2))
			return std::make_pair(l1, l2);
		
		return std::make_pair(l2, l1);
	}
}

namespace std
{
	template<>
	struct less<std::pair<svg::location, svg::location> >
	{
		bool operator()(std::pair<svg::location, svg::location> l1, std::pair<svg::location, svg::location> l2)
		{
			if (l1.first == l2.first)
				return std::less<svg::location>()(l1.second, l2.second);
			
			return std::less<svg::location>()(l1.first, l2.first);
		}
	};
}

namespace interpreter
{
	centerline_tracer::centerline_tracer(svg::document& vec, const cv::Mat& raster) : svg_doc(vec), pgm(raster)
	{}

	std::unique_ptr<svg::document> centerline_tracer::trace_centerlines()
	{
		std::vector<svg::location> locations;
		locations.reserve(svg_doc.points().size());
		for (auto& p : svg_doc.points())
			locations.push_back(p.first);

		auto clusters = clusterer(locations).clusterize();

		std::map<svg::location, std::size_t> point_to_cluster;
		for (auto loc : locations)
			point_to_cluster.insert(std::make_pair(loc, std::distance(clusters.begin(), std::min_element(clusters.begin(), clusters.end(), 
			    [loc](svg::location old_cluster, svg::location new_cluster) {return svg::cartesian_distance(loc, old_cluster) < svg::cartesian_distance(loc, new_cluster); }))));

		std::map<std::pair<svg::location, svg::location>, std::size_t> inter_cluster_connections;
		
		auto edges = svg_doc.edges();
		for (auto edge : edges)
			++inter_cluster_connections[ordered_pair(clusters[point_to_cluster.at(edge->start_point()->loc())], clusters[point_to_cluster.at(edge->end_point()->loc())])];

		auto res = std::make_unique<svg::document>(svg_doc.width(), svg_doc.height());
		for (auto& segments : inter_cluster_connections)
			if (segments.first.first != segments.first.second)
				res->add_segment(segments.first.first, segments.first.second);

		return res;
	}
}

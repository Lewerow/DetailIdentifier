#include <interpreter/centerline_tracer.h>
#include <interpreter/svg/document.h>

#include <interpreter/clusterer.h>

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

		auto res = std::make_unique<svg::document>(svg_doc.width(), svg_doc.height());
		for (auto cluster : clusters)
			res->add_point(cluster);

		return res;
	}
}

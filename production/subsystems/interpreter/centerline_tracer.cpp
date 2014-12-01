#include <interpreter/centerline_tracer.h>
#include <interpreter/svg/document.h>

namespace interpreter
{
	centerline_tracer::centerline_tracer(svg::document& vec, const cv::Mat& raster) : svg_doc(vec), pgm(raster)
	{}

	std::unique_ptr<svg::document> centerline_tracer::trace_centerlines()
	{
		auto important_points = svg_doc.points();


		auto res = std::make_unique<svg::document>(svg_doc.width(), svg_doc.height());

		return res;
	}
}

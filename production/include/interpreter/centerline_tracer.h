#ifndef CENTERLINE_TRACER_MDOismd89234mf9gr5jfkdmclsdcoxidmi24j8f43jgfirgnfdkfmsxndwe
#define CENTERLINE_TRACER_MDOismd89234mf9gr5jfkdmclsdcoxidmi24j8f43jgfirgnfdkfmsxndwe

#include <memory>

namespace svg
{
	class document;
}

namespace cv
{
	class Mat;
}

namespace interpreter
{
	class centerline_tracer
	{
	public:
		centerline_tracer(svg::document& vec, const cv::Mat& raster);
	    centerline_tracer& operator=(const centerline_tracer&) = delete;

		std::unique_ptr<svg::document> trace_centerlines();

	private:
		svg::document& svg_doc;
		const cv::Mat& pgm;
	};
}

#endif
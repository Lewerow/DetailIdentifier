#include <preprocessor/preprocessor.h>
#include <preprocessor/configuration.h>
#include <string>

#include <common/di_assert.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace preprocessor
{
	class preprocessor::impl
	{
	public:
		impl(std::shared_ptr<configuration> conf) : config(conf)
		{}

		std::unique_ptr<output> preprocess(std::unique_ptr<input> in)
		{
			cv::Mat img = cv::imread(config->input_filename());
			cv::Mat greyscale;
			cv::cvtColor(img, greyscale, CV_RGB2GRAY);

			cv::Mat binary;
			cv::threshold(greyscale, binary, 195, 255, CV_THRESH_BINARY);

//			cv::Mat erosion_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(3, 3));
//			cv::Mat eroded;
//			cv::erode(binary, eroded, erosion_element);

			cv::imwrite(config->workspace_path() + "preprocessed.pgm", binary);

			std::unique_ptr<output> out(std::make_unique<output>());
			out->filename = config->workspace_path() + "preprocessed.pgm";
			return out;
		}

		std::shared_ptr<configuration> config;
	};

	preprocessor::preprocessor(std::shared_ptr<configuration> config) : pimpl(std::make_unique<impl>(config))
	{}

	preprocessor::~preprocessor()
	{}

	std::unique_ptr<output> preprocessor::preprocess(std::unique_ptr<input> in)
	{
		return pimpl->preprocess(std::move(in));
	}
}
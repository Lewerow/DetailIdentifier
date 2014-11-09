#include <preprocessor/preprocessor.h>
#include <preprocessor/configuration.h>
#include <string>

#include <common/di_assert.h>

#include <opencv2/highgui/highgui.hpp>

namespace preprocessor
{
	class preprocessor::impl
	{
	public:
		std::string input_file;
		std::string workspace;
		logger::logger* log;
	};

	preprocessor::preprocessor(const configuration& config) : pimpl(std::make_unique<impl>())
	{
		pimpl->input_file = config.input_filename();
		pimpl->workspace = config.workspace_path();
		pimpl->log = &config.log();
	}

	preprocessor::~preprocessor()
	{}

	std::unique_ptr<output> preprocessor::preprocess(std::unique_ptr<input> in)
	{
		cv::Mat img = cv::imread(pimpl->input_file);
		cv::imwrite(pimpl->workspace + "preprocessed.pgm", img);

		return std::unique_ptr<output>();
	}
}
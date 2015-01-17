#include <preprocessor/preprocessor.h>
#include <preprocessor/configuration.h>

#include <executor/os_proxy.h>
#include <executor/exceptions.h>

#include <logger/logger.h>

#include <string>

#include <iostream>
#include <fstream>
#include <limits>

#include <common/di_assert.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <interpreter/svg/document.h>

#include <qgarlib/CleanedBinaryImage.h>
#include <qgarlib/Dist34BlackCCImage.h>
#include <qgarlib/DxfFile.h>
#include <qgarlib/GenImage.h>
#include <qgarlib/LabeledSkeletonImage.h>
#include <qgarlib/LinkedChainList.h>
#include <qgarlib/PbmFile.h>
#include <qgarlib/PgmFile.h>
#include <qgarlib/QgarApp.h>
#include <qgarlib/RWSegmentVector.h>
#include <qgarlib/ThresBinaryImage.h>


namespace
{

	namespace zhang_suen
	{
		// taken from: http://opencv-code.com/quick-tipst/implementation-of-thinning-algorithm-in-opencv/

		/**
		* Code for thinning a binary image using Zhang-Suen algorithm.
		*/

		/**
		* Perform one thinning iteration.
		* Normally you wouldn't call this function directly from your code.
		*
		* @param  im    Binary image with range = 0-1
		* @param  iter  0=even, 1=odd
		*/
		void thinningIteration(cv::Mat& im, int iter)
		{
			cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

			for (int i = 1; i < im.rows - 1; i++)
			{
				for (int j = 1; j < im.cols - 1; j++)
				{
					uchar p2 = im.at<uchar>(i - 1, j);
					uchar p3 = im.at<uchar>(i - 1, j + 1);
					uchar p4 = im.at<uchar>(i, j + 1);
					uchar p5 = im.at<uchar>(i + 1, j + 1);
					uchar p6 = im.at<uchar>(i + 1, j);
					uchar p7 = im.at<uchar>(i + 1, j - 1);
					uchar p8 = im.at<uchar>(i, j - 1);
					uchar p9 = im.at<uchar>(i - 1, j - 1);

					int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
						(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
						(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
						(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
					int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
					int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
					int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

					if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
						marker.at<uchar>(i, j) = 1;
				}
			}

			im &= ~marker;
		}

		/**
		* Function for thinning the given binary image
		*
		* @param  im  Binary image with range = 0-255
		*/
		void thinning(cv::Mat& im)
		{
			im /= 255;

			cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
			cv::Mat diff;

			do {
				thinningIteration(im, 0);
				thinningIteration(im, 1);
				cv::absdiff(im, prev, diff);
				im.copyTo(prev);
			} while (cv::countNonZero(diff) > 0);

			im *= 255;
		}
	}

	cv::Mat skeletize_zhang_suen(cv::Mat& img)
	{
		cv::Mat out(img.size(), true, CV_8UC1);
		cv::threshold(img, out, 127, 255, cv::THRESH_BINARY);
		cv::bitwise_not(out, out);
		zhang_suen::thinning(out);
		cv::bitwise_not(out, out);
		return out;
	}

	cv::Mat skeletize_simple(cv::Mat& img)
	{
		cv::Mat out(img.size(), true, CV_8UC1);
		cv::threshold(img, img, 127, 255, cv::THRESH_BINARY);
		cv::bitwise_not(img, img);
		
		cv::Mat temp(img.size(), CV_8UC1);
		cv::Mat skeleton(img.size(), CV_8UC1, cv::Scalar(0));
		cv::Mat orig;
		img.copyTo(orig);

		cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

		bool done = false;
		do
		{
			cv::morphologyEx(img, temp, cv::MORPH_OPEN, element);
			cv::bitwise_not(temp, temp);
			cv::bitwise_and(img, temp, temp);
			cv::bitwise_or(skeleton, temp, skeleton);
			cv::erode(img, img, element);

			double max;
			cv::minMaxLoc(img, 0, &max);
			done = (max == 0);
		} while (!done);

		return skeleton;
	}

	cv::Mat pick_wide_lines(const std::string& path)
	{
		int thin_line_width = 15;

		cv::Mat img = cv::imread(path, CV_LOAD_IMAGE_ANYDEPTH);
		cv::medianBlur(img, img, 5);
		cv::threshold(img, img, 127, 255, cv::THRESH_BINARY);

		cv::morphologyEx(img, img, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(thin_line_width, thin_line_width)));
		cv::dilate(img, img, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(thin_line_width, thin_line_width)));
		cv::erode(img, img, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(thin_line_width, thin_line_width)));

		return img;
	}
}

using namespace qgar;
using namespace std;
namespace preprocessor
{
	class preprocessor::impl
	{
	public:
		impl(std::shared_ptr<configuration> conf) : config(conf)
		{}

		std::unique_ptr<output> preprocess(std::unique_ptr<input> in)
		{
			cv::Mat greyscale = cv::imread(config->input_filename(), CV_LOAD_IMAGE_GRAYSCALE);

			cv::medianBlur(greyscale, greyscale, 9);
			std::string workspace = config->workspace_path();

			cv::imwrite(workspace + "smooth.pgm", greyscale);

			{
				// because OpenCV sucks at saving pbm
				// or I suck at OpenCV...
				qgar::PgmFile pgm((workspace + "smooth.pgm").c_str());
				qgar::PbmFile pbm((workspace + "smooth.pbm").c_str());
				pbm.write(qgar::ThresBinaryImage(pgm.read(), 170));
			}

			try
			{
			executor::os_proxy().call("QAtextGraphicSeparation.exe", { "-in", workspace + "smooth.pbm", "-outxt", workspace + "text.pbm", "-outg",
				workspace + "graphics.pbm", "-outu", workspace + "unknown.pbm", "-area", "10", "-elbbox", "100", "-elmaer", "2", "-dmaer", ".5" });
			}

			catch (executor::system_call_failure c)
			{
				config->log().error_log(std::string("System call failure: ") + c.what() + "\nContinuing, but expect crashes");
			}

			auto skeleton = skeletize_zhang_suen(greyscale);
			cv::imwrite(config->workspace_path() + "whole_skeleton.pgm", skeleton);

			auto wide_lines = pick_wide_lines(workspace + "graphics.pbm");
			cv::imwrite(config->workspace_path() + "wide_lines_only.pgm", wide_lines);

			auto wide_skeleton = skeletize_zhang_suen(wide_lines);
			cv::imwrite(config->workspace_path() + "wide_lines_only_skeleton.pgm", wide_skeleton);

			std::unique_ptr<output> out(std::make_unique<output>());
			out->wide_lines_filename = config->workspace_path() + "wide_lines_only_skeleton.pgm";
			out->original_filename = config->workspace_path() + "smooth.pgm";
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
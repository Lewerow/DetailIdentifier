#include <preprocessor/preprocessor.h>
#include <preprocessor/configuration.h>
#include <string>

#include <iostream>

#include <common/di_assert.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <voronoi/voronoi.h>
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

namespace
{
	cv::Mat skeletize(cv::Mat& img)
	{
		cv::Mat out(img.size(), true);
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
		/*
		cv::imshow("Skeleton", skeleton);
		cv::imshow("Orig", orig);
		cv::waitKey(0);

		return skeleton;

		VoronoiSkeleton skeleton;
		skeleton.thin(img, IMPL_GUO_HALL, false);
		return skeleton.get_skeleton();
*/
		//thinning(out);
		//cv::imshow("Skeleton", out);
		//cv::waitKey(0);
		return out;
	}
}

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

			auto skeleton = skeletize(greyscale);
//			cv::Mat erosion_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(3, 3));
//			cv::Mat eroded;
//			cv::erode(binary, eroded, erosion_element);

			cv::imwrite(config->workspace_path() + "skeleton.pgm", skeleton);

			cv::blur(skeleton, skeleton, cv::Size(5,5));
			cv::imwrite(config->workspace_path() + "blurred_skeleton.pgm", skeleton);
			cv::vector<cv::Vec4i> hough_lines;
			cv::HoughLinesP(skeleton, hough_lines, 1, CV_PI / 180, 50, 0);

			cv::Mat contours(greyscale.size(), CV_8UC1);
			for (auto& p : hough_lines)
				std::cout << "from (" << p[0] << "," << p[1] << ") to (" << p[2] << "," << p[3] << ")" << std::endl;

//			cv::imwrite(config->workspace_path() + "skeleton_hough.pgm", hough);
			
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
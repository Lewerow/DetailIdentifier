#include <preprocessor/preprocessor.h>
#include <preprocessor/configuration.h>
#include <string>

#include <iostream>
#include <fstream>
#include <limits>

#include <common/di_assert.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <interpreter/svg/document.h>

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
	}

	uchar get(const cv::Mat& original, int x, int y)
	{
		return original.at<uchar>(y, x);
	}
	
	struct location_iterator
	{
		location_iterator(svg::location start, double x_step, double y_step, double x_max, double y_max) : start_(start), x_step_(x_step), y_step_(y_step), x_max_(x_max), y_max_(y_max), step_(-10000)
		{}

		svg::location next()
		{
			++step_;
			return svg::location(start_.x + step_ * x_step_, start_.y + step_ * y_step_);
		}

		bool end()
		{
//			double current_x = start_.x + step_ * x_step_;
//			double current_y = start_.y + step_ * y_step_;
//			bool res = current_x > x_max_ || current_x < -0.5 || current_y > y_max_ || current_y < -0.5;
			return step_ > 10000;
		}

		svg::location start_;
		double x_step_;
		double y_step_;
		double x_max_;
		double y_max_;
		int step_;
	};

	std::vector<std::pair<svg::location, svg::location> > validate_any_lines(svg::location start, double x_step, double y_step, const cv::Mat& original)
	{
		const double min_line_lenght = 25; // 25 px/mm --> shortest line is 1 mm
		const double min_distance_between_lines = 8;

		std::vector<std::pair<svg::location, svg::location> > lines;
		std::pair<svg::location, svg::location> next_line;
		bool line_started = false;
		int empty_between = 0;
		int filled_between = 0;
		svg::location current_location(start);
		
		for (location_iterator it(start, x_step, y_step, original.cols, original.rows); !it.end(); current_location = it.next())
		{
			if (current_location.x < 0 || current_location.y < 0 || current_location.x >= original.cols || current_location.y >= original.rows)
				continue;

			if (get(original, current_location.x, current_location.y) > 0)
			{
				++filled_between;
				if (!line_started)
				{
					line_started = true;
					next_line.first = current_location;
				}
				else
				{
					filled_between += empty_between;
					empty_between = 0;
				}
			}
			else
			{
				++empty_between;
				if (empty_between > min_distance_between_lines && line_started)
				{
					line_started = false;

					if (filled_between > min_line_lenght)
					{
						next_line.second = svg::location(current_location.x, current_location.y - empty_between);
						lines.push_back(next_line);
					}
					filled_between = 0;
				}
			}
		}

		return lines;
	}

	struct boundaries
	{
		boundaries(int up, int down) : up_(up), down_(down)
		{}

		bool fit(double d)
		{
			return d >= down_ && d <= up_;
		}
		int up_;
		int down_;
	};

	std::vector<std::pair<svg::location, svg::location> > validate_lines(float rho, float theta, const cv::Mat& original)
	{
		double a = cv::cos(theta);
		double b = cv::sin(theta);
		double x_step = a;
		double y_step = b;

		svg::location start = svg::location(rho * a, rho * b);/*
		if (a >= 0 && b <= 0) // IV quarter
		{
			start.x -= a * start.y;
			start.y = 0;
			y_step = -y_step;
		}
		else if (a <= 0 && b >= 0) // II quarter
		{
			start.y -= b * start.x; 
			start.x = 0;
			x_step = -x_step;
		}
		else // I quarter -> III is impossible
		{
			boundaries x(0, original.cols);

			start.x -= a * start.y;
			start.y = 0;
			x_step = -x_step;
			y_step = -y_step;
			
			if (!x.fit(start.x))
			{
				start.y -= b * (x.up_ - start.x);
				start.x = x.up_;
				x_step = -x_step;
				y_step = -y_step;
			}
		}
		*/
//		return validate_any_lines(start, -y_step, x_step, original);

		return{ { svg::location(start.x + 1000 * (-b), start.y + 1000 * a), svg::location(start.x - 1000 * (-b), start.y - 1000 * a) }};
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

			cv::GaussianBlur(greyscale, greyscale, cv::Size(7, 7), 5);
			cv::imwrite(config->workspace_path() + "smooth.pgm", greyscale);

			auto skeleton = skeletize(greyscale);
			cv::imwrite(config->workspace_path() + "skeleton.pgm", skeleton);

			cv::vector<cv::Vec2f> hough_lines;
			cv::HoughLines(skeleton, hough_lines, 1, CV_PI / 180, 170, 0);


		    svg::document svg_doc(img.cols, img.rows);
			for (auto& p : hough_lines)
			{
				auto lines = validate_lines(p[0], p[1], greyscale);

				for (auto line : lines)
				{
					svg_doc.add_segment(line.first, line.second);
					std::cout << "from: (" << line.first.x << "," << line.first.y << ") to (" << line.second.x << "," << line.second.y << ")" << std::endl;
				}

//    				std::cout << "coords: ( a = " << cv::cos(p[1]) << ", b = " << cv::sin(p[1]) << " offset: " << p[0] << std::endl;
			}
				
//			cv::imwrite(config->workspace_path() + "skeleton_hough.pgm", hough);
			
			std::ofstream str(config->workspace_path() + "vectors.svg");
			str << svg_doc.dump();
			str.flush();

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
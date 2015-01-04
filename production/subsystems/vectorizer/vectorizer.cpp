#include <vectorizer/vectorizer.h>
#include <vectorizer/configuration.h>

#include <logger/logger.h>

#include <executor/os_proxy.h>
#include <executor/exceptions.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <interpreter/svg/document.h>
#include <interpreter/svg/location.h>

#include <fstream>

namespace
{
	struct point
	{
		int x;
		int y;

		int first_crossing;
		int second_crossing;
	};

	struct line_params
	{
		// ax + by + c = 0
		double a;
		double b;
		double c;
	};

	point get_crossing(line_params l1, line_params l2)
	{
		point res;
		res.x = -1;
		res.y = -1;

		double wmain = l1.a * l2.b - l2.a * l1.b;
		if (wmain == 0)
			return res;

		double wx = -l1.c * l2.b + l2.c * l1.b;
		double wy = -l2.c * l1.a + l1.c * l2.a;

		res.x = wx / wmain;
		res.y = wy / wmain;

		return res;
	}

	bool is_on_image(point p, cv::Size s)
	{
		return p.x >= 0 && p.y >= 0 && p.x < s.height && p.y < s.width;
	}

	cv::Point top_left(point p, cv::Size s)
	{
		cv::Point res;
		if (p.x < 5)
			res.x = 0;
		else
			res.x = p.x - 5;
		
		if (p.y > s.width - 5)
			res.y = s.width - 1;
		else
			res.y = p.y + 5;

		return res;
	}
	
	cv::Point bottom_right(point p, cv::Size s)
	{
		cv::Point res;
		if (p.x > s.height - 5)
			res.x = s.height - 1;
		else
			res.x = p.x + 5;

		if (p.y < 5)
			res.y = 0;
		else
			res.y = p.y - 5;

		return res;
	}

	double f(line_params l, int x)
	{
		return -l.a / l.b * x - l.c / l.b;
	}

	std::pair<std::vector<point>, std::vector<std::vector<std::size_t> > > detect_crossing_points(const std::vector<line_params>& hough_lines, cv::Size size)
	{
		cv::Mat points_matrix = cv::Mat::zeros(size, CV_32S);
		std::fill(points_matrix.data, points_matrix.data + size.height * size.width, 0);

		std::vector<point> crossings;
		std::vector<std::vector<std::size_t> > collinear_points(hough_lines.size());
		crossings.emplace_back();
		for (auto i = hough_lines.begin(); i != hough_lines.end(); ++i)
		{
			for (auto j = i + 1; j != hough_lines.end(); ++j)
			{
				point cross = get_crossing(*i, *j);
				if (is_on_image(cross, size))
				{
					auto pair = std::make_pair(top_left(cross, size), bottom_right(cross, size));
					int value = points_matrix.at<int>(cv::Point(cross.x, cross.y));
					if (value != 0)
						continue;

					cross.first_crossing = std::distance(hough_lines.begin(), i);
					cross.second_crossing = std::distance(hough_lines.begin(), j);
					crossings.push_back(cross);
					collinear_points[cross.first_crossing].push_back(crossings.size()-1);
					collinear_points[cross.second_crossing].push_back(crossings.size()-1);
					cv::rectangle(points_matrix, pair.first, pair.second, /*crossings.size() - 1*/255,CV_FILLED);
				}
			}
		}

		for (auto& v : collinear_points)
			std::sort(v.begin(), v.end(), [&crossings](int p1, int p2){return crossings[p1].x < crossings[p2].x ||
				(crossings[p1].x == crossings[p2].x && crossings[p1].y < crossings[p2].y); });

		cv::imwrite("workspace/crossing_points.pgm", points_matrix);
		return std::make_pair(crossings, collinear_points);
	}


	line_params translate_to_typical(cv::Vec2f line)
	{
		line_params res;
		res.a = cv::cos(line[1]);
		res.b = cv::sin(line[1]);
		res.c = -line[0];
		return res;
	}
	
	bool is_line_between(point p1, point p2, cv::Mat& img)
	{
		cv::Mat temp = cv::Mat::zeros(img.size(), CV_8UC1);
		cv::line(temp, cv::Point(p1.x, p1.y), cv::Point(p2.x, p2.y), cv::Scalar(255));
		
		cv::Mat comp;
		cv::bitwise_and(temp, img, comp);
		
		cv::Mat out;
		cv::absdiff(temp, comp, out);
				
		return cv::countNonZero(out) < 20;
	}

	void draw_svg(std::pair<std::vector<point>, std::vector<std::vector<std::size_t> > > crossings, std::string filename, std::string workspace)
	{
		cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_ANYDEPTH);
		auto& points = crossings.first;

		svg::document doc(img.rows, img.cols);

		for (auto lines : crossings.second)
		{
			for (std::size_t i = 1; i < lines.size(); ++i)
			{
				if (is_line_between(points[lines[i]], points[lines[i - 1]], img))
					doc.add_segment(svg::location(points[lines[i]].x, points[lines[i]].y), svg::location(points[lines[i - 1]].x, points[lines[i - 1]].y));
			}
		}
		
		std::ofstream str((workspace + "vectorized.svg").c_str());
		str << doc.dump();
	}
}

namespace vectorizer
{
	class vectorizer::impl
	{
	public:
		impl(std::shared_ptr<configuration> conf) : config(conf)
		{}

		std::unique_ptr<output> vectorize(std::unique_ptr<input> in)
		{

			cv::Mat img = cv::imread(in->wide_lines_filename, CV_LOAD_IMAGE_ANYDEPTH);
			cv::threshold(img, img, 127, 255, cv::THRESH_BINARY);
			cv::vector<cv::Vec2f> hough_lines;
			cv::HoughLines(img, hough_lines, 1, CV_PI / 180, 120, 0);

			cv::Mat found_lines(img.size(), false);
			for (size_t i = 0; i < hough_lines.size(); i++)
			{
				float rho = hough_lines[i][0], theta = hough_lines[i][1];
				cv::Point pt1, pt2;
				double a = cos(theta), b = sin(theta);
				double x0 = a*rho, y0 = b*rho;
				pt1.x = cvRound(x0 + 5000 * (-b));
				pt1.y = cvRound(y0 + 5000 * (a));
				pt2.x = cvRound(x0 - 5000 * (-b));
				pt2.y = cvRound(y0 - 5000 * (a));
				cv::line(found_lines, pt1, pt2, cv::Scalar(0, 0, 255), 3, CV_AA);
			}
			cv::imwrite(config->workspace_path() + "detected_lines.pgm", found_lines);


			std::vector<line_params> params(hough_lines.size());
			std::transform(hough_lines.begin(), hough_lines.end(), params.begin(), translate_to_typical);

			auto crossings(detect_crossing_points(params, cv::Size(img.rows, img.cols)));

			draw_svg(crossings, in->original_filename, config->workspace_path());
			
			std::unique_ptr<output> out(std::make_unique<output>());
			out->svg_filename = config->workspace_path() + "vectorized.svg";

			return out;
		}

		std::shared_ptr<configuration> config;
	};

	vectorizer::vectorizer(std::shared_ptr<configuration> config) : pimpl(std::make_unique<impl>(config))
	{}
	
	vectorizer::~vectorizer() 
	{}

	std::unique_ptr<output> vectorizer::vectorize(std::unique_ptr<input> in)
	{
		return pimpl->vectorize(std::move(in));
	}
}
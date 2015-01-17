#include <vectorizer/vectorizer.h>
#include <vectorizer/configuration.h>

#include <logger/logger.h>

#include <executor/os_proxy.h>
#include <executor/exceptions.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <interpreter/svg/document.h>
#include <interpreter/svg/location.h>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <iostream>

template<>
struct std::hash<cv::Point2i>
{
	size_t operator()(const cv::Point2i& p) const
	{
		std::hash<int> parent;
		return (parent(p.x) + 0x9e3779b9) ^ (parent(p.y) << 7);
	}
};

namespace
{
	void dump_points(cv::Size imgSize, cv::vector<cv::vector<cv::Point2i> >& points, std::string path)
	{
		cv::Mat matContour(imgSize, CV_8UC1);
		for (size_t k = 0; k < points.size(); ++k)
		{
			const cv::vector<cv::Point2i>& contour = points[k];
			for (size_t k2 = 0; k2 < contour.size(); ++k2)
			{
				const cv::Point2i& p = contour[k2];
				matContour.at<uint8_t>(p) = 255;
			}
		}

		imwrite(path, matContour);
	}

	double dist(cv::Point2i a, cv::Point2i b)
	{
		double x = a.x - b.x;
		double y = a.y - b.y;

		return std::sqrt(x*x + y*y);
	}


	void seed_neighborhood(cv::Mat index, cv::Point2i center, int value, int radius)
	{
		int start_x = center.x - radius;
		int end_x = center.x + radius;

		int start_y = center.y - radius;
		int end_y = center.y + radius;

		if (start_x < 0)
			start_x = 0;
		if (start_y < 0)
			start_y = 0;
		if (end_x >= index.cols)
			end_x = index.cols - 1;
		if (end_y >= index.rows)
			end_y = index.rows - 1;

		
		for (int i = start_x; i <= end_x; ++i)
		{
			for (int j = start_y; j <= end_y; ++j)
				index.at<int>(cv::Point(i, j)) = value;
		}
	}

	bool edge_already_exists(const std::vector<std::pair<std::size_t, std::size_t> >& u, std::pair<std::size_t, std::size_t> e)
	{
		for (auto a : u)
		{
			if ((a.first == e.first && a.second == e.second) || (a.second == e.first && a.first == e.second))
				return true;
		}

		return false;
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
			cv::bitwise_not(img, img);

			cv::vector<cv::vector<cv::Point2i> > chained_points;
			cv::findContours(img, chained_points, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
			std::cout << "Found: " << chained_points.size() << " contours" << std::endl;

			dump_points(img.size(), chained_points, config->workspace_path() + "contours.pgm");
			
			cv::vector<cv::vector<cv::Point2i> > vertices;
			for (std::size_t i = 0; i < chained_points.size(); ++i)
			{
				cv::vector<cv::Point2i> vertex_points;
				cv::approxPolyDP(chained_points[i], vertex_points, 15, true);
				vertices.push_back(vertex_points);
				std::cout << "From: " << chained_points[i].size() << " points detected: " << vertex_points.size() << "needed for contour" << std::endl;
			}

			std::vector<cv::Point2i> points;
			std::vector<std::pair<std::size_t, std::size_t> > edges;

			int START_SEED = 255;
			cv::Mat minimal(img.size(), CV_32SC1, cv::Scalar(START_SEED));
			int SEED_RADIUS = 15;
			for (std::size_t i = 0; i < vertices.size(); ++i)
			{
				if (vertices[i].size() == 2)
					continue;

				int index = minimal.at<int>(vertices[i][0]);
				if (index == START_SEED)
				{
					index = points.size();
					points.push_back(vertices[i][0]);
				}
				seed_neighborhood(minimal, vertices[i][0], index, SEED_RADIUS);

				for (std::size_t j = 1; j < vertices[i].size(); ++j)
				{
					int index2 = minimal.at<int>(vertices[i][j]);
					if (index2 == START_SEED)
					{
						index2 = points.size();
						points.push_back(vertices[i][j]);
					}

					seed_neighborhood(minimal, vertices[i][j], index2, SEED_RADIUS);

					if (index == index2)
						continue;
					
					auto e = std::make_pair(index, index2);
					if (!edge_already_exists(edges, e))
						edges.push_back(e);

					index = index2;
				}
			}
			cv::imwrite(config->workspace_path() + "minimal_points.pgm", minimal);

			std::cout << "Total: " << points.size() << " vertices" << std::endl;
			for (std::size_t i = 0; i < points.size(); ++i)
				std::cout << i << ".) (" << points[i].x << "," << points[i].y << ")" << std::endl;

			std::cout << "Total: " << edges.size() << " edges" << std::endl;
			for (std::size_t i = 0; i < edges.size(); ++i)
				std::cout << i << ".) (" << edges[i].first << "," << edges[i].second << ")" << std::endl;

			cv::Mat vectorized = cv::Mat::zeros(minimal.size(), CV_8UC1);
			for (std::size_t i = 0; i < edges.size(); ++i)
				cv::line(vectorized, points[edges[i].first], points[edges[i].second], 255);

			cv::imwrite(config->workspace_path() + "vectorized.pgm", vectorized);

			cv::Mat sss = cv::imread(in->wide_lines_filename, CV_LOAD_IMAGE_ANYDEPTH);
			std::unique_ptr<output> out(std::make_unique<output>());
			out->skeleton_image = sss;
			out->vertices = points;
			out->edges = edges;

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
#include <interpreter/interpreter.h>
#include <interpreter/configuration.h>
#include <interpreter/centerline_tracer.h>

#include <executor/os_proxy.h>

#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/segment.h>
#include <interpreter/projection.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/lexical_cast.hpp>

#include <cmath>
#include <algorithm>
#include <iostream>
#include <set>
#include <numeric>

namespace
{
	cv::vector<interpreter::projection> separate_projections( const std::vector<cv::Point2i>& vertices, std::vector<std::pair<std::size_t, std::size_t> > edges)
	{
		cv::vector<interpreter::projection> projections;
		while (!edges.empty())
		{
			std::set<std::size_t> next_projection;
			auto e = edges.back();
			edges.pop_back();

			next_projection.insert(e.first);
			next_projection.insert(e.second);

			interpreter::projection c;
			bool no_change = false;
			while (!no_change)
			{
				no_change = true;
				auto t = std::find_if(edges.begin(), edges.end(), [&next_projection](std::pair<std::size_t, std::size_t> p){return std::find_if(next_projection.begin(), next_projection.end(),
					[p](std::size_t t){return t == p.first || t == p.second; }) != next_projection.end(); });

				if (t != edges.end())
				{
					next_projection.insert(t->first);
					next_projection.insert(t->second);

					c.edges.push_back(std::make_pair(vertices[t->first], vertices[t->second]));
					edges.erase(t);

					no_change = false;
				}
			}

			for (auto t : next_projection)
				c.vertices.push_back(vertices[t]);

			projections.push_back(c);
		}

		return projections;
	}

	struct projection_direction
	{
		double A;
		double B;
		double C;
	};
	
	void prune(std::vector<interpreter::projection>& projections)
	{
		std::size_t i = 0;
		while (i < projections.size())
		{
			cv::Point2f center = projections[i].bounding_box.center;

			bool removed_any = false;
			for (std::size_t j = 0; j < projections.size(); ++j)
			{
				if (i == j)
					continue;

				if (center.inside(projections[j].bounding_box.boundingRect()))
				{
					removed_any = true;
					if (projections[j].bounding_box.size.area() > projections[i].bounding_box.size.area())
					{
						projections.erase(projections.begin() + i);
						break;
					}
					else
					{
						projections.erase(projections.begin() + j);
						--j;
					}
				}

			}

			if (!removed_any)
				++i;
		}

	}

	projection_direction calculate_direction(const interpreter::projection& c1, const interpreter::projection& c2)
	{
		double x_diff = c1.bounding_box.center.x - c2.bounding_box.center.x;
		double y_diff = c1.bounding_box.center.y - c2.bounding_box.center.y;

		if (std::abs(x_diff) < 1 && std::abs(y_diff) < 1)
			return projection_direction{ 1, 0, -c1.bounding_box.center.x };
		else if (std::abs(x_diff) < 1)
			return projection_direction{ 0, 1, -c1.bounding_box.center.y };
		else if (std::abs(y_diff) < 1)
			return projection_direction{ 0, 0, 0 };
		else
		{
			double ang = std::atan(-y_diff / x_diff);
			return projection_direction{ std::cos(ang), std::sin(ang), -(c1.bounding_box.center.x * std::cos(ang) + c1.bounding_box.center.y * std::sin(ang))};

		}
	}

	std::vector<std::vector<projection_direction> > calculate_projection_directions(const std::vector<interpreter::projection>& projections)
	{
		std::vector<std::vector<projection_direction> > directions(projections.size());
		for (std::size_t i = 0; i < directions.size(); ++i)
			directions[i].resize(projections.size());

		for (std::size_t i = 0; i < directions.size(); ++i)
		{
			directions[i][i] = projection_direction{ 0, 0, 0 };
			for (std::size_t j = i + 1; j < directions.size(); ++j)
			{
				projection_direction d = calculate_direction(projections[i], projections[j]);
			}
		}
	}
}

namespace interpreter
{
	class interpreter::impl
	{
	public:
		impl(std::shared_ptr<configuration> conf) : config(conf)
		{}
		
		std::unique_ptr<output> generate_interpretation(std::unique_ptr<input> in)
		{
			auto out = std::make_unique<output>();

			cv::vector<projection> potential_projections = separate_projections(in->vertices,  in->edges);

			for (auto& c : potential_projections)
				c.bounding_box = cv::minAreaRect(c.vertices);	

			prune(potential_projections);

			for (std::size_t i = 0; i < potential_projections.size(); ++i)
			{
				cv::Mat with_box = cv::Mat::zeros(in->skeleton_image.size(), CV_8UC1);
				cv::Point2f rect_points[4];
				potential_projections[i].bounding_box.points(rect_points);
				for (int j = 0; j < 4; j++)
					cv::line(with_box, rect_points[j], rect_points[(j + 1) % 4], 255);

				for (auto e : potential_projections[i].edges)
					cv::line(with_box, e.first, e.second, 255);

				cv::imwrite(config->workspace_path() + "with_boxes_projection_" + boost::lexical_cast<std::string>(i)+".pgm", with_box);
			}

			auto projection_directions = calculate_projection_directions(potential_projections);
			auto main_projection = select_main_projection()

			return std::make_unique<output>();
		}

		std::shared_ptr<configuration> config;
	};

	interpreter::interpreter(std::shared_ptr<configuration> config) : pimpl(std::make_unique<impl>(config))
	{}

	interpreter::~interpreter()
	{}

	std::unique_ptr<output> interpreter::generate_interpretation(std::unique_ptr<input> in)
	{
		return pimpl->generate_interpretation(std::move(in));
	}
}

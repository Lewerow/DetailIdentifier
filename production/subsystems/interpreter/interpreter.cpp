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
#include <iomanip>
#include <iterator>
#include <numeric>

namespace
{
	cv::vector<interpreter::projection> separate_projections( const std::vector<cv::Point2i>& vertices, std::vector<std::pair<std::size_t, std::size_t> > edges)
	{
		std::cout << "Projections separation started" << std::endl;
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

			for (auto e : c.edges)
			{
				c.cedges.push_back(std::make_pair(std::distance(c.vertices.begin(), std::find(c.vertices.begin(), c.vertices.end(), e.first)), 
					std::distance(c.vertices.begin(), std::find(c.vertices.begin(), c.vertices.end(), e.second))));
			}

			projections.push_back(c);
		}
		std::cout << "Projections separation finished" << std::endl;

		return projections;
	}
	
	void merge(interpreter::projection& target, const interpreter::projection& src)
	{
		std::size_t previous_size = target.vertices.size();
		target.vertices.insert(target.vertices.end(), src.vertices.begin(), src.vertices.end());

		for (std::size_t k = 0; k < src.edges.size(); ++k)
		{
			target.edges.push_back(src.edges[k]);
			target.cedges.push_back(std::make_pair(src.cedges[k].first + previous_size, src.cedges[k].second + previous_size));
		}
	}

	bool inside(const cv::RotatedRect& rect, cv::Point2f p)
	{
		cv::Point2f second(p.x - rect.center.x, p.y - rect.center.y);
		cv::Point2f transformed(second.x * std::cos(rect.angle) + second.y * std::sin(rect.angle), -second.x * std::sin(rect.angle) + second.y * std::cos(rect.angle));

		return std::abs(2 * transformed.x) < rect.size.width && std::abs(2 * transformed.y) < rect.size.height;
	}

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

				if (inside(projections[j].bounding_box, center))
				{
					removed_any = true;
					if (projections[j].bounding_box.size.area() > projections[i].bounding_box.size.area())
					{
						merge(projections[j], projections[i]);
						projections.erase(projections.begin() + i);
						break;
					}
					else
					{
						merge(projections[i], projections[j]);
						projections.erase(projections.begin() + j);
						--j;
					}
				}
			}

			if (!removed_any)
				++i;
		}

	}

	interpreter::projection_direction calculate_direction(const interpreter::projection& c1, const interpreter::projection& c2)
	{
		std::cout << "Calculating directions between centers : " << c1.bounding_box.center << " and " << c2.bounding_box.center << std::endl;

		double x_diff = c1.bounding_box.center.x - c2.bounding_box.center.x;
		double y_diff = c1.bounding_box.center.y - c2.bounding_box.center.y;

		interpreter::projection_direction res;

		if (std::abs(x_diff) < 1 && std::abs(y_diff) < 1)
			res = interpreter::projection_direction{ 0, 0 };
		else if (std::abs(x_diff) < 1)
			res = interpreter::projection_direction{ 0, c1.bounding_box.center.x };
		else if (std::abs(y_diff) < 1)
			res = interpreter::projection_direction{ CV_PI / 2, c1.bounding_box.center.y };
		else
		{
			double ang = std::atan(-x_diff / y_diff);
			res = interpreter::projection_direction{ ang, c1.bounding_box.center.x * std::cos(ang) + c1.bounding_box.center.y * std::sin(ang) };
		}

		std::cout << "Calculated ANG : " << res.alpha << " P: " << res.p << std::endl;
		std::cout << "Distance to c1 center: " << res.get_distance_to(c1.bounding_box.center) << " to c2 center: " << res.get_distance_to(c2.bounding_box.center) << std::endl;

		return res;
	}


	cv::Point2i far_behind(cv::Point2i center, interpreter::projection_direction dir)
	{
		double p = center.x * std::cos(dir.alpha) + center.y * std::sin(dir.alpha);
		return cv::Point2i(-2000, static_cast<int>(p / std::sin(dir.alpha) + 2000 / std::tan(dir.alpha)));
	}

	cv::Point2i far_before(cv::Point2i center, interpreter::projection_direction dir)
	{
		double p = center.x * std::cos(dir.alpha) + center.y * std::sin(dir.alpha);
		return cv::Point2i(20000, static_cast<int>(p / std::sin(dir.alpha) - 20000 / std::tan(dir.alpha)));
	}

	std::vector<std::vector<interpreter::projection_direction> > calculate_projection_directions(const std::vector<interpreter::projection>& projections, cv::Mat& common, std::string name)
	{
		std::vector<std::vector<interpreter::projection_direction> > directions(projections.size());
		for (std::size_t i = 0; i < directions.size(); ++i)
			directions[i].resize(projections.size());

		for (std::size_t i = 0; i < directions.size(); ++i)
		{
			directions[i][i] = interpreter::projection_direction{ 0, 0 };
			for (std::size_t j = i + 1; j < directions.size(); ++j)
			{
				interpreter::projection_direction d = calculate_direction(projections[i], projections[j]);
				directions[i][j] = d;
				directions[j][i] = d;

				std::cout << "Projection direction between: " << i << " and " << j << ": " << "Ang: " << directions[i][j].alpha << ", P: " << directions[i][j].p << std::endl;

				cv::line(common, far_behind(projections[i].bounding_box.center, directions[i][j]), far_before(projections[j].bounding_box.center, directions[i][j]), 255, 2);
			}
		}

		cv::imwrite(name, common);

		return directions;
	}
	
	bool are_perpenticular_or_parallel(interpreter::projection_direction d1, interpreter::projection_direction d2)
	{
		return (std::abs(d1.alpha) - std::abs(d2.alpha)) < 0.01 && std::abs(d1.p - d2.p) < 0.01;
	}

	bool all_perpenticular_or_parallel(const std::vector<interpreter::projection_direction>& dirs)
	{
		for (std::size_t i = 1; i < dirs.size(); ++i)
		{
			if (!are_perpenticular_or_parallel(dirs[i], dirs[0]))
				return false;
		}

		return true;
	}

	std::size_t select_main_projection(const std::vector<std::vector<interpreter::projection_direction> >& projection_directions)
	{
		return 0;
		/*
		for (std::size_t i = 0; i < projection_directions.size(); ++i)
		{
			if (all_perpenticular_or_parallel(projection_directions[i]))
				return i;
		}

		assert(!"No main projection!");
		return -1;*/
	}

	double calculate_distance(cv::Point2d center, interpreter::projection_direction parallel, cv::Point2d target)
	{
		interpreter::projection_direction line{ parallel.alpha, -std::sin(parallel.alpha) * center.x + std::cos(parallel.alpha) * center.y };
		return std::abs(-std::sin(parallel.alpha) * target.x + std::cos(parallel.alpha) * target.y - line.p);
	}

	void assign_coordinate_systems(std::vector<interpreter::projection>& projections, const std::vector<std::vector<interpreter::projection_direction> >& directions, std::size_t main_projection)
	{
		cv::Point2i main_zero = projections[main_projection].vertices[0];
		projections[main_projection].s.zero_point_image_location = main_zero;
		projections[main_projection].s.has_x = true;
		projections[main_projection].s.has_y = true;
		projections[main_projection].s.has_z = false;

		projections[main_projection].s.y_line = directions[0][1];
		projections[main_projection].s.y_line.p -= 10000;
		projections[main_projection].s.x_line = { projections[main_projection].s.y_line.alpha + CV_PI / 2, -10000 };
		projections[main_projection].s.z_line = { 0, 0 };

		std::cout << "Projection: " << main_projection << " zero point: " << projections[main_projection].s.zero_point_image_location << std::endl;
//		std::cout << "Proof: " << projections[main_projection].s.get_location_of(main_zero) << std::endl;

		assert(projections.size() == 2);

		for (std::size_t k = 0; k < projections.size(); ++k)
		{
			if (k == main_projection)
				continue;

			std::size_t min = 0;
			double min_dist = std::numeric_limits<double>::max();
			for (std::size_t l = 0; l < projections[k].vertices.size(); ++l)
			{
				double dist = calculate_distance(main_zero, directions[main_projection][k], projections[k].vertices[l]);
				std::cout << "Distance " << projections[k].vertices[l] << " to " << main_zero << " by ANG:" << std::setprecision(6) << directions[main_projection][k].alpha << " -> " << dist << std::endl;
				if (dist < min_dist)
				{
					min = l;
					min_dist = dist;
				}
			}

			projections[k].s.zero_point_image_location = projections[k].vertices[min];
			projections[k].s.has_x = false;
			projections[k].s.has_y = true;
			projections[k].s.has_z = true;

			projections[k].s.x_line = { 0, 0 };
			projections[k].s.y_line = directions[0][1];
			projections[k].s.y_line.p -= 10000;
			projections[k].s.z_line = { projections[main_projection].s.y_line.alpha + CV_PI / 2, -10000 };

			std::cout << "Projection: " << k << " zero point: " << projections[k].vertices[min] << std::endl;
//			std::cout << "Proof: " << projections[k].s.get_location_of(projections[k].vertices[min]) << std::endl;
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
			cv::vector<projection> potential_projections = separate_projections(in->vertices,  in->edges);

			for (auto& c : potential_projections)
				c.bounding_box = cv::minAreaRect(c.vertices);	

			prune(potential_projections);

			cv::Mat common = cv::Mat::zeros(in->skeleton_image.size(), CV_8UC1);
			for (std::size_t i = 0; i < potential_projections.size(); ++i)
			{
				cv::Mat with_box = cv::Mat::zeros(in->skeleton_image.size(), CV_8UC1);
				cv::Point2f rect_points[4];
				potential_projections[i].bounding_box.points(rect_points);
				for (int j = 0; j < 4; j++)
					cv::line(with_box, rect_points[j], rect_points[(j + 1) % 4], 255);

				for (auto e : potential_projections[i].edges)
					cv::line(with_box, e.first, e.second, 255);

				cv::bitwise_or(common, with_box, common);
				cv::imwrite(config->workspace_path() + "with_boxes_projection_" + boost::lexical_cast<std::string>(i)+".pgm", with_box);
			}

			auto projection_directions = calculate_projection_directions(potential_projections, common, config->workspace_path() + "with_projection_directions.pgm");
			auto main_projection = select_main_projection(projection_directions);

			assign_coordinate_systems(potential_projections, projection_directions, main_projection);

			auto out = std::make_unique<output>();
			out->projection_directions = projection_directions;
			out->main_projection = main_projection;
			out->projections = potential_projections;
			out->skeleton_image = in->skeleton_image;

			return out;
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

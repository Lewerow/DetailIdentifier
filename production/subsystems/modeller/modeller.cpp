#include <modeller/modeller.h>
#include <modeller/configuration.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/viz/viz3d.hpp>

#include <boost/lexical_cast.hpp>

#include <map>
#include <vector>
#include <iostream>
#include <set>

const int MAX_DIFF = 70;


namespace
{
	std::ostream& operator<<(std::ostream& str, cv::Point3d p)
	{
		return str << "(" << p.x << "," << p.y << "," << p.z << ")";
	}

	struct model3d
	{
		std::vector<cv::Point3d> points;
		std::vector<std::pair<std::size_t, std::size_t> > edges;
		std::vector<std::pair<cv::Point3d, cv::Point3d> > pedges;
	};

	void load(cv::viz::Viz3d& viz, const model3d& model)
	{
		std::cout << "Displaying model" << std::endl;

		static int x = 0;
		for (std::size_t i = 0; i < model.points.size(); ++i)
		{
			std::cout << "Adding to model point: " << model.points[i]<< std::endl;

			std::vector<cv::viz::Color> c({ cv::viz::Color::amethyst(), cv::viz::Color::yellow(), cv::viz::Color::gray(), cv::viz::Color::cherry(), cv::viz::Color::apricot() });
			auto ball = cv::viz::WSphere(model.points[i], 30, 15, c[x % c.size()]);
			viz.showWidget("point" + boost::lexical_cast<std::string>(++x), ball);
		}

		std::map<std::size_t, std::set<std::size_t> > edges;
		for (std::size_t i = 0; i < model.edges.size(); ++i)
		{
			if (edges[model.edges[i].first].count(model.edges[i].second) != 0)
				continue;

			edges[model.edges[i].first].insert(model.edges[i].second);
			edges[model.edges[i].second].insert(model.edges[i].first);

			std::cout << "Line between: " << model.points[model.edges[i].first] << " and " << model.points[model.edges[i].second] << std::endl;
			auto line = cv::viz::WLine(model.points[model.edges[i].first], model.points[model.edges[i].second], cv::viz::Color::red());
			viz.showWidget("line" + boost::lexical_cast<std::string>(++x), line);
		}
	}

	double calculate_distance(cv::Point2d center, interpreter::projection_direction parallel, cv::Point2d target)
	{
		interpreter::projection_direction line{ parallel.alpha , std::cos(parallel.alpha) * center.x + std::sin(parallel.alpha) * center.y};
		return std::abs(std::cos(line.alpha) * target.x + std::sin(line.alpha) * target.y - line.p);
	}

	cv::Point3d merge_points(cv::Point3d p1, interpreter::projection& pr1, cv::Point3d p2, interpreter::projection& pr2)
	{
		cv::Point3d p;
		
		if (!(pr1.s.has_x || pr2.s.has_x))
			std::cerr << "None of the points has x coordinate!" << "(" << p1.x << "," << p1.y << "," << p1.z << "), (" << p2.x << "," << p2.y << "," << p2.z << ")" << std::endl;

		if (pr1.s.has_x)
			p.x = p1.x;
		else if (pr2.s.has_x)
			p.x = p2.x;

		if (pr1.s.has_x && pr2.s.has_x && std::abs(p1.x - p2.x) > MAX_DIFF)
			std::cerr << "Wrong points were used! Too big x difference between same point: " << "(" << p1.x << "," << p1.y << "," << p1.z << "), (" << p2.x << "," << p2.y << "," << p2.z << ")" << std::endl;

		if (!(pr1.s.has_y || pr2.s.has_y))
			std::cerr << "None of the points has y coordinate!" << "(" << p1.x << "," << p1.y << "," << p1.z << "), (" << p2.x << "," << p2.y << "," << p2.z << ")" << std::endl;

		if (pr1.s.has_y)
			p.y = p1.y;
		else if (pr2.s.has_y)
			p.y = p2.y;

		if (pr1.s.has_y && pr2.s.has_y && std::abs(p1.y - p2.y) > MAX_DIFF)
			std::cerr << "Wrong points were used! Too big y difference between same point: " << "(" << p1.x << "," << p1.y << "," << p1.z << "), (" << p2.x << "," << p2.y << "," << p2.z << ")" << std::endl;

		if (!(pr1.s.has_z || pr2.s.has_z))
			std::cerr << "None of the points has z coordinate!" << "(" << p1.x << "," << p1.y << "," << p1.z << "), (" << p2.x << "," << p2.y << "," << p2.z << ")" << std::endl;

		if (pr1.s.has_z)
			p.z = p1.z;
		else if (pr2.s.has_z)
			p.z = p2.z;

		if (pr1.s.has_z && pr2.s.has_z && std::abs(p1.z - p2.z) > MAX_DIFF)
			std::cerr << "Wrong points were used! Too big z difference between same point: " << p1 << " and " << p2 << std::endl;

		
		return p;
	}

	cv::Point3d coords_point(interpreter::projection& p1, std::size_t p1i, interpreter::projection& p2, std::size_t p2i)
	{
		cv::Point3d v1 = p1.s.get_location_of(p1.vertices[p1i]);
		cv::Point3d v2 = p2.s.get_location_of(p2.vertices[p2i]);

//		std::cout << "Sanity check: " << p1.s.get_location_of(p1.s.zero_point_image_location) << "Zero: " << 
//			p1.s.zero_point_image_location << " " << std::boolalpha << p1.s.has_x << "," << p1.s.has_y << "," << p1.s.has_z << std::endl;

		std::cout << p2.s.x_line.alpha << " " << p2.s.y_line.alpha << p2.s.z_line.alpha << std::endl;
		std::cout << "Mixing up: " << v1 << " with " << v2 << std::endl;

		cv::Point3d res = merge_points(v1, p1, v2, p2);
		std::cout << "Received: " << res << std::endl;

		return res;
	}

	std::vector<std::size_t> filter_needed(const std::set<std::size_t>& possible, std::set<std::size_t>& pts, const std::vector<cv::Point3d>& ref)
	{
		const double ALLOWED_DISTANCE = 40;
		std::vector<std::size_t> accepted;
		for (auto s : possible)
		{
			for (auto g : pts)
			{
				bool x = std::abs(ref[g].x - ref[s].x) < ALLOWED_DISTANCE;
				bool y = std::abs(ref[g].y - ref[s].y) < ALLOWED_DISTANCE;
				bool z = std::abs(ref[g].z - ref[s].z) < ALLOWED_DISTANCE;

				bool result = (x && y) || (x && z) || (z && y);
				std::cout << "Analyzing: " << ref[g] << " vs " << ref[s] << " verdict: " << result << std::endl;
				if (result)
					accepted.push_back(s);
			}
		}

		return accepted;//std::vector<std::size_t>(possible.begin(), possible.end());
	}

	struct parents
	{
		std::size_t proj1;
		std::size_t pt1;

		std::size_t proj2;
		std::size_t pt2;
	};

	model3d build_model(modeller::input& in)
	{
		model3d mod;

		std::map < std::size_t, parents > points_parents;
		
		std::cout << "Starting to build 3D model. Using " << in.projections.size() << " projections" << std::endl;
		for (std::size_t i = 0; i < in.projections.size(); ++i)
		{
			std::cout << "Projection: " << i << " points: " << in.projections[i].vertices.size() << std::endl;

			for (std::size_t k = i + 1; k < in.projections.size(); ++k)
			{
//				std::cout << "Projection: " << k << " points: " << in.projections[k].vertices.size() << std::endl;
				for (std::size_t j = 0; j < in.projections[i].vertices.size(); ++j)
				{
					for (std::size_t l = 0; l < in.projections[k].vertices.size(); ++l)
					{
						auto dist = calculate_distance(in.projections[i].vertices[j], in.projection_directions[i][k], in.projections[k].vertices[l]);
						if (dist < MAX_DIFF)
						{
							std::cout << "Between: " << in.projections[i].vertices[j] << "and" << in.projections[k].vertices[l] << " distance: " << dist << " parallel: " << in.projection_directions[i][k].alpha << std::endl;
							cv::Point3d p = coords_point(in.projections[i], j, in.projections[k], l);
							mod.points.push_back(p);

							points_parents[mod.points.size() - 1] = parents{ i, j, k, l };
							std::cout << "Added to model: " << p << std::endl;
						}
					}
				}
			}
		}

		std::cout << "Searching for edges" << std::endl;
		std::cout << "Found: " << points_parents.size() << " points" << std::endl;

		for (auto it = points_parents.begin(); it != points_parents.end(); ++it)
		{
			const auto& first_edges = in.projections[it->second.proj1].cedges;
			
			auto it2 = it;
			for (++it2; it2 != points_parents.end(); ++it2)
			{
				std::vector<std::pair<std::size_t, std::size_t> > second_edges;
				std::size_t first_first;
				std::size_t first_second;
				std::size_t second_first;
				std::size_t second_second;

				if (it2->second.proj1 == it->second.proj1)
				{
					second_edges = in.projections[it2->second.proj2].cedges;
					first_first = it->second.pt1;
					first_second = it2->second.pt1;
					second_first = it->second.pt2;
					second_second = it2->second.pt2;
				}

				else if (it2->second.proj1 == it->second.proj2)
				{
					second_edges = in.projections[it2->second.proj1].cedges;
					first_first = it->second.pt1;
					first_second = it2->second.pt2;
					second_first = it->second.pt2;
					second_second = it2->second.pt1;
				}
					
				else
					continue;


				bool connected_on_first = it->second.pt1 == it2->second.pt1 || std::find(first_edges.begin(), first_edges.end(), std::make_pair(first_first, first_second)) != first_edges.end() ||
					std::find(first_edges.begin(), first_edges.end(), std::make_pair(first_second, first_first)) != first_edges.end();

				bool connected_on_second = it->second.pt2 == it2->second.pt2 || std::find(first_edges.begin(), first_edges.end(), std::make_pair(second_first, second_second)) != first_edges.end() ||
					std::find(first_edges.begin(), first_edges.end(), std::make_pair(second_second, second_first)) != first_edges.end();

				if (connected_on_first && connected_on_second)
					mod.edges.push_back(std::make_pair(it->first, it2->first));
			}
		}
				
		std::cout << "Edges found, displaying" << std::endl;
		return mod;
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
}

namespace modeller
{
	modeller::modeller(std::shared_ptr<configuration> config) : cfg(config)
	{}

	modeller::~modeller()
	{}

	void modeller::draw_cast_lines(const input& in)
	{
		cv::Mat disp = cv::Mat::zeros(in.skeleton_image.size(), CV_8UC1);

		for (std::size_t i = 0; i < in.projections.size(); ++i)
		{
			for (std::size_t j = 0; j < in.projections[i].vertices.size(); ++j)
				cv::rectangle(disp, cv::Rect(in.projections[i].vertices[j].x - 20, in.projections[i].vertices[j].y - 20, 40, 40), 123, CV_FILLED);
		
			for (std::size_t k = i + 1; k < in.projections.size(); ++k)
			{
				std::cout << "Projection direction between: " << i << " and " << k << ": " << "Ang: " << in.projection_directions[i][k].alpha << ", P: " << in.projection_directions[i][k].p << std::endl;

				cv::line(disp, far_behind(in.projections[i].bounding_box.center, in.projection_directions[i][k]), far_before(in.projections[i].bounding_box.center, in.projection_directions[i][k]), 127, 2);

				for (std::size_t j = 0; j < in.projections[i].vertices.size(); ++j)
				{
					cv::line(disp, far_behind(in.projections[i].vertices[j], in.projection_directions[i][k]), far_before(in.projections[i].vertices[j], in.projection_directions[i][k]), 255, 2);
				}
			}
		}

		cv::imwrite(cfg->workspace_path() + "cast_lines.pgm", disp);
	}

	std::unique_ptr<output> modeller::generate_model(std::unique_ptr<input> in)
	{
		draw_cast_lines(*in);
		model3d model = build_model(*in);

		cv::viz::Viz3d viz("3D Model");
		load(viz, model);
		viz.spin();
		viz.saveScreenshot(cfg->workspace_path() + "screenshot_3D.png");
		
		return std::make_unique<output>();
	}

	std::shared_ptr<configuration> cfg;
}
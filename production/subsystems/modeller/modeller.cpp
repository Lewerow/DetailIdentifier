#include <modeller/modeller.h>
#include <modeller/configuration.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/viz/viz3d.hpp>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);

#include <vtkImageData.h>
#include <vtkSTLWriter.h>
#include <vtkPLYWriter.h>
#include <vtkTriangleFilter.h>
#include <vtkPLYReader.h>
#include <vtkSTLReader.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAppendFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkVoxelModeller.h>
#include <vtkMarchingCubes.h>
#include <vtkUndirectedGraph.h>
#include <vtkGraphLayoutFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkPolygon.h>
#include <vtkCellArray.h>


#include <boost/lexical_cast.hpp>

#include <map>
#include <vector>
#include <iostream>
#include <set>

const int MAX_DIFF = 30;


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

	const int CONTOUR_DIFF = 40;	
	std::vector<std::vector<std::size_t> > align_contours_x(model3d& model)
	{
		std::vector<std::vector<std::size_t> > res;
		std::vector<int> available;
		for (std::size_t i = 0; i < model.points.size(); ++i)
		{
			bool found = false;
			for (std::size_t j = 0; j < available.size(); ++j)
			{
				if (std::abs(model.points[i].x - available[j]) < CONTOUR_DIFF)
				{
					model.points[i].x = available[j];
					res[j].push_back(i);
					found = true;
					break;
				}
			}

			if (!found)
			{
				available.push_back(model.points[i].x);
				res.push_back(std::vector<std::size_t>());
				res.back().push_back(i);
			}
		}

		return res;
	}
	std::vector<std::vector<std::size_t> > align_contours_y(model3d& model)
	{
		std::vector<std::vector<std::size_t> > res;
		std::vector<int> available;
		for (std::size_t i = 0; i < model.points.size(); ++i)
		{
			bool found = false;
			for (std::size_t j = 0; j < available.size(); ++j)
			{
				if (std::abs(model.points[i].y - available[j]) < CONTOUR_DIFF)
				{
					model.points[i].y = available[j];
					res[j].push_back(i);
					found = true;
					break;
				}
			}

			if (!found)
			{
				available.push_back(model.points[i].y);
				res.push_back(std::vector<std::size_t>());
				res.back().push_back(i);
			}
		}

		return res;
	}
	std::vector<std::vector<std::size_t> > align_contours_z(model3d& model)
	{
		std::vector<std::vector<std::size_t> > res;
		std::vector<int> available;
		for (std::size_t i = 0; i < model.points.size(); ++i)
		{
			bool found = false;
			for (std::size_t j = 0; j < available.size(); ++j)
			{
				if (std::abs(model.points[i].z - available[j]) < CONTOUR_DIFF)
				{
					model.points[i].z = available[j];
					res[j].push_back(i);
					found = true;
					break;
				}
			}

			if (!found)
			{
				available.push_back(model.points[i].z);
				res.push_back(std::vector<std::size_t>());
				res.back().push_back(i);
			}
		}

		return res;
	}

	void addFaces(const model3d& model, std::vector<std::vector<std::size_t> > per_plane_points, vtkSmartPointer<vtkAppendPolyData>& data)
	{
		std::cout << "Total planes: " << per_plane_points.size() << " plane. Searching for faces" << std::endl;
		std::cout << "All edges: " << model.edges.size() << ". Searching for faces" << std::endl;

		for (std::size_t i = 0; i < model.edges.size(); ++i)
		{
			std::cout << model.points[model.edges[i].first] << " to " << model.points[model.edges[i].second] << std::endl;
		}

		for (std::size_t i = 0; i < per_plane_points.size(); ++i)
		{
			std::cout << "Analyzing: " << i << " plane. Searching for faces" << std::endl;
			std::cout << "Available points: " <<per_plane_points[i].size() << std::endl;
			for (std::size_t k = 0; k < per_plane_points[i].size(); ++k)
				std::cout << k << ".) " << model.points[per_plane_points[i][k]] << std::endl;

			std::vector<std::size_t> current;
			std::set<std::size_t> used;
			while (per_plane_points[i].size() != used.size())
			{
//				std::cout << "Used: " << used.size() << " available: " << per_plane_points[i].size() << " ongoing: " << current.size() << std::endl;

				if (current.empty())
				{
					if (used.size() == per_plane_points[i].size())
						break;

					auto k = std::find_if(per_plane_points[i].begin(), per_plane_points[i].end(), [&used, &per_plane_points, i](std::size_t j){return used.count(j) == 0; });

					if (k == per_plane_points[i].end())
						break;

					current.push_back(*k);
					used.insert(*k);
				}
				
				bool found = false;
				for (std::size_t j = 0; j < per_plane_points[i].size(); ++j)
				{
//					std::cout << "For: " << per_plane_points[i][j] << " found: " << std::boolalpha << (std::find(current.begin(), current.end(), per_plane_points[i][j]) != current.end()) << std::endl;

					if (std::find(current.begin(), current.end(), per_plane_points[i][j]) != current.end() && (per_plane_points[i][j] != current[0] || current.size() < 4))
						continue;
					
					if (std::find_if(model.edges.begin(), model.edges.end(), [&current, &per_plane_points, i, j](std::pair<std::size_t, std::size_t> p)
						{return (p.first == current.back() && p.second == per_plane_points[i][j]) || (p.second == current.back() && p.first == per_plane_points[i][j]); }) != model.edges.end())
					{
//						std::cout << "Adding next point: " << per_plane_points[i][j] << std::endl;
						found = true;
						current.push_back(per_plane_points[i][j]);
						used.insert(per_plane_points[i][j]);
						break;
					}
				}

//				std::cout << "2. Used: " << used.size() << " available: " << per_plane_points[i].size() << " ongoing: " << current.size() << std::endl;
				if (current.size() > 2 && current.front() == current.back())
				{
					current.pop_back();
					found = false;
				}

				if (current.size() == 4 && (std::find_if(model.edges.begin(), model.edges.end(), [&current, &per_plane_points, i](std::pair<std::size_t, std::size_t> p)
				{return (p.first == current.back() && p.second == current.front()) || (p.second == current.back() && p.first == current.front()); }) != model.edges.end()))
				found = false;

//				std::cout << "Found next point ? " << std::boolalpha << found << " current points: " << std::endl;
/*				for (std::size_t k = 0; k < current.size(); ++k)
				{
					std::cout << k << ".) " << model.points[current[k]] << std::endl;
				}
*/
				if (!found && current.size() > 2)
				{
					std::cout << "Found a face! Yay! Number of points: " << current.size() << std::endl;

					vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
					vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
					polygon->GetPointIds()->SetNumberOfIds(current.size());
					const int visibility_rate = 10;
					for (std::size_t k = 0; k < current.size(); ++k)
					{
						points->InsertNextPoint(model.points[current[k]].x / visibility_rate, model.points[current[k]].y / visibility_rate, model.points[current[k]].z / visibility_rate);
						std::cout << k << ".) " << model.points[current[k]] << std::endl;
						polygon->GetPointIds()->SetId(k, k);
					}
				
					vtkSmartPointer<vtkCellArray> cell = vtkSmartPointer<vtkCellArray>::New();
					cell->InsertNextCell(polygon);

					vtkSmartPointer<vtkPolyData> polys = vtkSmartPointer<vtkPolyData>::New();
					polys->SetPoints(points);
					polys->SetPolys(cell);

					vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
					triangleFilter->SetInputData(0, polys);
					triangleFilter->Update();
					
					data->AddInputConnection(triangleFilter->GetOutputPort());
				}
				if (!found)
					current.clear();
			}

			if (current.size() > 0)
			{
				std::cout << "Unable to create face from following " << current.size() << " points: " << std::endl;
				for (std::size_t k = 0; k < current.size(); ++k)
				{
					std::cout << k << ".) " << model.points[current[k]] << std::endl;
				}

				vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
				vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
				polygon->GetPointIds()->SetNumberOfIds(current.size());
				const int visibility_rate = 10;
				for (std::size_t k = 0; k < current.size(); ++k)
				{
					points->InsertNextPoint(model.points[current[k]].x / visibility_rate, model.points[current[k]].y / visibility_rate, model.points[current[k]].z / visibility_rate);
					std::cout << k << ".) " << model.points[current[k]] << std::endl;
					polygon->GetPointIds()->SetId(k, k);
				}

				vtkSmartPointer<vtkCellArray> cell = vtkSmartPointer<vtkCellArray>::New();
				cell->InsertNextCell(polygon);

				vtkSmartPointer<vtkPolyData> polys = vtkSmartPointer<vtkPolyData>::New();
				polys->SetPoints(points);
				polys->SetPolys(cell);

				vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
				triangleFilter->SetInputData(0, polys);
				triangleFilter->Update();

				data->AddInputConnection(triangleFilter->GetOutputPort());
			}
		}
	}

	void load(model3d& model, std::string target)
	{
		std::cout << "Displaying model" << std::endl;

		static int x = 0;
		
		vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
		vtkSmartPointer<vtkAppendPolyData> data = vtkSmartPointer<vtkAppendPolyData>::New();

		std::vector<std::vector<std::size_t> > x_contours = align_contours_x(model);
		std::vector<std::vector<std::size_t> > y_contours = align_contours_y(model);
		std::vector<std::vector<std::size_t> > z_contours = align_contours_z(model);
		addFaces(model, x_contours, data);
		addFaces(model, y_contours, data);
		addFaces(model, z_contours, data);

		stlWriter->AddInputConnection(data->GetOutputPort());
		stlWriter->SetFileName(target.c_str());
		stlWriter->Write();

		// Read and display for verification
		vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(target.c_str());
		reader->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper =	vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(reader->GetOutputPort());

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetLineWidth(100);

		vtkSmartPointer<vtkRenderer> renderer =	vtkSmartPointer<vtkRenderer>::New();
		vtkSmartPointer<vtkRenderWindow> renderWindow =	vtkSmartPointer<vtkRenderWindow>::New();
		renderWindow->AddRenderer(renderer);
		vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =	vtkSmartPointer<vtkRenderWindowInteractor>::New();
		renderWindowInteractor->SetRenderWindow(renderWindow);

		renderer->AddActor(actor);
		renderer->SetBackground(0, 0, 0);

		renderWindow->Render();
		renderWindowInteractor->Start();
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

		load(model, cfg->output_filename());
		
		return std::make_unique<output>();
	}

	std::shared_ptr<configuration> cfg;
}
#include <interpreter/interpreter.h>
#include <interpreter/configuration.h>

#include <executor/os_proxy.h>

#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/segment.h>

#include <opencv2/highgui/highgui.hpp>

#include <cmath>
#include <algorithm>
#include <iostream>

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
			out->dxf_filename = config->workspace_path() + "interpreted.dxf";
			out->svg_filename = config->workspace_path() + "interpreted.svg";
			
            auto docs = get_input(std::move(in));
			
            auto svg_out = trace_centerlines(std::move(docs.first), std::move(docs.second));
            out->layers.insert(std::make_pair("image", std::move(svg_out)));

			config->os_proxy().save_file(out->dxf_filename, "");
			return out;
		}

        std::pair<std::unique_ptr<svg::document>, cv::Mat> get_input(std::unique_ptr<input> in)
        {
    		std::string svg_input_content = config->os_proxy().load_file(in->svg_filename);
			std::string pgm_input_content = config->os_proxy().load_file(in->pgm_input_filename);
			
			svg::reader reader;
			auto svg_doc = reader.parse(svg_input_content);
			auto pgm_doc = cv::imdecode(cv::_InputArray(pgm_input_content.c_str(), pgm_input_content.size()), CV_LOAD_IMAGE_GRAYSCALE);

            assert(pgm_doc.channels() == 1);

            return std::make_pair(std::move(svg_doc), pgm_doc);
        }

        std::unique_ptr<svg::document> trace_centerlines(std::unique_ptr<svg::document> svg_doc, cv::Mat&& pgm_doc)
        {
            auto points = svg_doc->points();
            auto line_delimiters = get_lines_vertices(points, pgm_doc);
            return pointize(std::make_pair(svg_doc->width(), svg_doc->height()), std::move(line_delimiters));
        }

        typedef std::vector<svg::location> line_vertices;
        typedef std::vector<svg::location> line_endpoint;    

        std::unique_ptr<svg::document> pointize(std::pair<double, double> dims, std::vector<line_vertices> vertices)
        {
            std::vector<svg::location> locations;
            std::vector<std::pair<std::size_t, std::size_t> > segments;;
           
            for(auto& line : vertices)
            {
                std::pair<line_endpoint, line_endpoint> endpoints = deduce_endpoints(line);
                validate_endpoints(endpoints);
                locations.push_back(middle(endpoints.first));
                locations.push_back(middle(endpoints.second));
                
                segments.push_back(std::make_pair(locations.size() - 1, locations.size() - 2));
            }
            
            std::vector<svg::location> clusters = clusterize(locations);
            std::vector<std::size_t> cluster_ids(locations.size()); 
            std::transform(locations.begin(), locations.end(), cluster_ids.begin(), cluster_ids.end(), [&clusters](svg::location l) {
                    std::pair<std::size_t, double> closest(std::numeric_limits<std::size_t>::max(), std::numeric_limits<double>::max());
                    for(std::size_t i = 0; i < clusters.size(); ++i)
                    {
                    double dist = cartesian_distance(l, clusters[i]);
                    if(dist < closest.second)
                    closest = std::make_pair(i, dist);
                    }
                    return closest.first;        
                    });
             
            for(auto& s: segments)
                s = std::make_pair(cluster_ids[s.first], cluster_ids[s.second]);


            auto svg_doc = std::make_unique<svg::document>(dims.first, dims.second);
                  
            for(auto& s: segments)
            {
                svg::point start_point(clusters[s.first]);
                svg::point end_point(clusters[s.second]);
                svg_doc->add_point(start_point);
                svg_doc->add_point(end_point);

                svg_doc->at(end_point.loc()).add_edge(std::make_shared<svg::segment>(svg_doc->at(end_point.loc()), svg_doc->at(start_point.loc())));
                svg_doc->at(start_point.loc()).add_edge(std::make_shared<svg::segment>(svg_doc->at(start_point.loc()), svg_doc->at(end_point.loc())));
            }
            return svg_doc;
        }

        std::vector<svg::location> clusterize(const std::vector<svg::location>& locs)
        {
            svg::vector<svg::location> clusters;
            clusters.push_back(locs[0]);
            
           const double acceptable_spread = 400;
            const int iterations = 10;
            do
            {
                auto second = find_furthest(locs, locs[0]);
                clusters.push_back(locs[second]);
                
                clusters = adapt(clusters, locs, iterations)
                auto spreads = calculate_spreads(clusters, locs);                
            } while(!std::all_of(spreads.begin(), spreads.end(), [acceptable_spread](double spread){return spread < acceptable_spread;}));
        }

        std::vector<double> calculate_spreads(const std::vector<svg::location>& clusters, const std::vector<svg::location>& locs)
        {
            
        }

        svg::location middle(line_endpoint p)
        {
            auto xs = [](double x, svg::location l2){return x + l2.x;};
            auto ys = [](double y, svg::location l2){return y + l2.y;};
            
            return svg::location(std::accumulate(p.begin(), p.end(), 0, xs) / p.size(), std::accumulate(p.begin(), p.end(), 0, ys) / p.size());
        }

        std::size_t find_furthest(const std::vector<svg::location>& loc, svg::location l1)
        {
            std::vector<double> distances;
            for(std::size_t i = 0; i < line.size(); ++i)
                distances.push_back(cartesian_distance(l1, line[i]));
            auto furthest = std::max_element(distances.begin(), distances.end());
            return std::distance(distances.begin(), furthest);
        }

        std::pair<line_endpoint, line_endpoint> deduce_endpoints(line_vertices line)
        {
            line_endpoint start;
            line_endpoint end;

            start.push_back(line[0]);
            end.push_back(line[find_furthest(line, line[0])]);
          
            for(std::size_t i = 0; i < line.size(); ++i)
            {
                if(cartesian_distance(start[0], line[i]) > cartesian_distance(end[0], line[i]))
                    end.push_back(line[i]);
                else
                    start.push_back(line[i]);
            }

            return std::make_pair(start, end);
        }

        double bigger(double a, double b)
        {
             return a > b ? a : b;
        }

        void validate_endpoints(std::pair<line_endpoint, line_endpoint>& line)
        {
            const double threshold = 400;
            double first_spread = spread(line.first);
            double second_spread = spread(line.second);
            assert((first_spread < bigger(2*second_spread, threshold)) && second_spread < bigger(2*first_spread, threshold) && "Entities other than lines are totally not supported yet");
        }

        double spread(const line_endpoint& p)
        {
            double max_x = p[0].x;
            double max_y = p[0].y;
            double min_x = p[0].x;
            double min_y = p[0].y;
            
            for(std::size_t i = 1; i < p.size(); ++i)
            {
                if(p[i].x > max_x)
                    max_x = p[i].x;
                else if(p[i].x < min_x)
                    min_x = p[i].x;

                if(p[i].y > max_y)
                    max_y = p[i].y;
                else if(p[i].y < min_y)
                    min_y = p[i].y;
            }

            return (max_x - min_x) * (max_y - min_y);
        }

        double cartesian_distance(svg::location l1, svg::location l2)
        {
            double xs = l1.x - l2.x;
            double ys = l1.y - l2.y;
            return std::sqrt(xs*xs + ys*ys);
        }

        std::vector<line_vertices> get_lines_vertices(const std::map<svg::location, svg::point>& points, const cv::Mat& pgm)
        {
            std::vector<svg::location> init;
            return add_next_vertex(points.begin(), points, pgm, init);
        }
        
        std::vector<line_vertices> add_next_vertex(std::map<svg::location, svg::point>::const_iterator start, const std::map<svg::location, svg::point>& points, const cv::Mat& pgm, std::vector<svg::location>& vertices)
        {
            const std::size_t vertices_per_line = 4;
            std::vector<line_vertices> res;
            for(auto it = start; it != points.end(); ++it)
            {
                if(!are_one_line(vertices, it->first, pgm))
                    continue;
                
                auto local_vertices = vertices;
                local_vertices.push_back(it->first);
                
                if(local_vertices.size() == vertices_per_line)
                {
                    res.push_back(local_vertices);
                    continue;
                }
                
                auto below_results = add_next_vertex(std::next(it), points, pgm, local_vertices);
                res.insert(res.end(), below_results.begin(), below_results.end());
            }

            return res;
        }

        bool are_one_line(const std::vector<svg::location>& vertices, svg::location next_vertex, const cv::Mat& pgm)
        {
            return std::all_of(vertices.begin(), vertices.end(), std::bind(&impl::are_line, this, std::placeholders::_1, next_vertex, std::cref(pgm)));
        }
       
        bool are_line(svg::location lhs, svg::location rhs, const cv::Mat& pgm)
        {
            int start_x = lhs.x;
            int start_y = lhs.y;

            int end_x = rhs.x;
            int end_y = rhs.y;

            int x_diff = end_x - start_x;
            int y_diff = end_y - start_y;

 
            std::size_t max_steps = std::abs(x_diff) + std::abs(y_diff) + 1;
            double x_step = static_cast<double>(x_diff) / max_steps;
            double y_step = static_cast<double>(y_diff) / max_steps;

            for(std::size_t step = 0; step < max_steps; ++step)
            {
                int current_x = start_x + step * x_step;
                int current_y = start_y + step * y_step;

                if(end_x == current_x && end_y == current_y)
                    return true;

                else if(!line_nearby(pgm, current_x, current_y))
                    return false;
            }
             
            return true;
        }

        bool line_nearby(const cv::Mat& pgm, std::size_t row, std::size_t col)
        {
            uint8_t* data = pgm.data;
            for(std::size_t x = row - 2; x < row + 2; ++x)
            {
                for(std::size_t y = col - 2; y < col + 2; ++y)
                {
                    if(data[x + y * pgm.cols] < 255)
                        return true;
                }
            }

            return false;
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

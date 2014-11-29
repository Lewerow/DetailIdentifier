#include <interpreter/interpreter.h>
#include <interpreter/configuration.h>

#include <executor/os_proxy.h>

#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>
#include <interpreter/svg/segment.h>

#include <opencv2/highgui/highgui.hpp>

#include <cmath>

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

            return std::make_pair(std::move(svg_doc), pgm_doc);
        }

        std::unique_ptr<svg::document> trace_centerlines(std::unique_ptr<svg::document> svg_doc, cv::Mat&& pgm_doc)
        {
            auto points = svg_doc->points();
            auto line_delimiters = get_lines_vertices(points, pgm_doc);
            return pointize(std::make_pair(svg_doc->width(), svg_doc->height()), std::move(line_delimiters));
        }

        typedef std::tuple<svg::location, svg::location, svg::location, svg::location> line_vertices;
        typedef std::pair<svg::location, svg::location> line_endpoint;    

        std::unique_ptr<svg::document> pointize(std::pair<double, double> dims, std::vector<line_vertices> vertices)
        {
            auto svg_doc = std::make_unique<svg::document>(dims.first, dims.second);
            
            for(auto& line : vertices)
            {
                std::pair<line_endpoint, line_endpoint> endpoints = deduce_endpoints(line);
                validate_endpoints(endpoints);
                svg::point start_point(middle(endpoints.first));
                svg::point end_point(middle(endpoints.second));
                svg_doc->add_point(start_point);
                svg_doc->add_point(end_point);

                svg_doc->at(end_point.loc()).add_edge(std::make_shared<svg::segment>(svg_doc->at(end_point.loc()), svg_doc->at(start_point.loc())));
                svg_doc->at(start_point.loc()).add_edge(std::make_shared<svg::segment>(svg_doc->at(start_point.loc()), svg_doc->at(end_point.loc())));
            }

            return svg_doc;
        }

        svg::location middle(line_endpoint p)
        {
            return svg::location((p.first.x + p.second.x)/2, (p.first.y + p.second.y)/2);
        }

        std::pair<line_endpoint, line_endpoint> deduce_endpoints(line_vertices line)
        {
            std::vector<double> distances;
            distances.push_back(cartesian_distance(std::get<0>(line), std::get<1>(line)));
            distances.push_back(cartesian_distance(std::get<0>(line), std::get<2>(line)));
            distances.push_back(cartesian_distance(std::get<0>(line), std::get<3>(line)));

            if(distances[0] < distances[1] && distances[0] < distances[2])
                return std::make_pair(line_endpoint(std::get<0>(line), std::get<1>(line)), line_endpoint(std::get<2>(line), std::get<3>(line)));
            else if(distances[1] < distances[2])
                return std::make_pair(line_endpoint(std::get<0>(line), std::get<2>(line)), line_endpoint(std::get<1>(line), std::get<3>(line)));
            else
                return std::make_pair(line_endpoint(std::get<0>(line), std::get<3>(line)), line_endpoint(std::get<1>(line), std::get<2>(line)));
        }

        void validate_endpoints(std::pair<line_endpoint, line_endpoint>& line)
        {
            double first_len = cartesian_distance(line.first.first, line.first.second);
            double second_len = cartesian_distance(line.second.first, line.second.second);

            assert(first_len < 2*second_len && second_len < 2*first_len && "Entities other than lines are totally not supported yet");
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
            std::vector<line_vertices> res;
            for(auto it = start; it != points.end(); ++it)
            {
                if(!are_one_line(vertices, it->first, pgm))
                    continue;
                
                auto local_vertices = vertices;
                local_vertices.push_back(it->first);
                
                if(local_vertices.size() == 4)
                {
                    res.push_back(make_vertex(local_vertices));
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
            return true;
        }

        line_vertices make_vertex(const std::vector<svg::location>& vec)
        {
            assert(vec.size() == 4);
            return line_vertices(vec[0], vec[1], vec[2], vec[3]);
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

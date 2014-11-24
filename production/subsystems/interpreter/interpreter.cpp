#include <interpreter/interpreter.h>
#include <interpreter/configuration.h>

#include <executor/os_proxy.h>

#include <interpreter/svg/reader.h>
#include <interpreter/svg/document.h>

#include <opencv2/highgui/highgui.hpp>

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
			
			std::string svg_input_content = config->os_proxy().load_file(in->svg_filename);
			std::string pgm_input_content = config->os_proxy().load_file(in->pgm_input_filename);
			
			svg::reader reader;
			auto svg_doc = reader.parse(svg_input_content);
			auto pgm_doc = cv::imdecode(cv::_InputArray(pgm_input_content.c_str(), pgm_input_content.size()), CV_LOAD_IMAGE_GRAYSCALE);

			out->layers.insert(std::make_pair("image", std::move(svg_doc)));

			config->os_proxy().save_file(out->dxf_filename, "");
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
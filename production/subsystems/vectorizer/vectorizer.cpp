#include <vectorizer/vectorizer.h>
#include <vectorizer/configuration.h>

#include <logger/logger.h>

#include <executor/os_proxy.h>
#include <executor/exceptions.h>

namespace vectorizer
{
	class vectorizer::impl
	{
	public:
		impl(std::shared_ptr<configuration> conf) : config(conf)
		{}

		bool run_potrace(const std::vector<std::string>& args)
		{
			try
			{
				config->os_proxy().call(config->potrace_location(), args);
			}
			catch (executor::system_call_failure s)
			{
				config->log().error_log(std::string("System call failed: ") + s.what());
				return false;
			}

			return true;
		}

		bool convert_to_svg(const std::string& in_file, const std::string& out_file)
		{
			return run_potrace({ "--svg", "-a", "0", "-o", out_file, "--", in_file });
		}

		bool convert_to_dxf(const std::string& in_file, const std::string& out_file)
		{
			return run_potrace({ "-b", "dxf", "-o", out_file, "--", in_file });
		}

		bool convert_to_pgm(const std::string& in_file, const std::string& out_file)
		{
			return run_potrace({ "--pgm", "-o", out_file, "--", in_file });
		}

		std::unique_ptr<output> vectorize(std::unique_ptr<input> in)
		{
			std::unique_ptr<output> out(std::make_unique<output>());

			out->svg_filename = config->workspace_path() + "vectorized.svg";
			if (!convert_to_svg(in->filename, out->svg_filename))
				throw std::runtime_error("Vectorization (conversion to svg) failed");

			out->dxf_filename = config->workspace_path() + "vectorized.dxf";
			if (!convert_to_dxf(in->filename, out->dxf_filename))
				throw std::runtime_error("Vectorization (conversion to dxf) failed");

			out->pgm_input_filename = config->workspace_path() + "vectorization_input.pgm";
			config->os_proxy().copy(in->filename, out->pgm_input_filename);

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
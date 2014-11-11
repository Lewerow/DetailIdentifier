#include <interpreter/interpreter.h>
#include <interpreter/configuration.h>

#include <executor/os_proxy.h>

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
			out->dxf_filename = "";
			out->svg_filename = config->workspace_path() + "interpreted.svg";

			config->os_proxy().copy(in->svg_filename, out->svg_filename);

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
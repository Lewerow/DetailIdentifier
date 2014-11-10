#include <executor/application.h>
#include <executor/processor.h>

#include <preprocessor/preprocessor.h>
#include <vectorizer/vectorizer.h>
#include <interpreter/interpreter.h>
#include <modeller/modeller.h>

#include <preprocessor/configuration.h>
#include <vectorizer/configuration.h>
#include <interpreter/configuration.h>
#include <modeller/configuration.h>
#include <executor/configuration.h>

#include <common/preprocessor_interface_types.h>
#include <common/modeller_interface_types.h>

namespace executor
{
	class whole_application::impl
	{
	public:
		impl(std::shared_ptr<preprocessor::configuration> preprocessor_config_, std::shared_ptr<vectorizer::configuration> vectorizer_config_,
			std::shared_ptr<interpreter::configuration> interpreter_config_, std::shared_ptr<modeller::configuration> modeller_config_,
			std::shared_ptr<executor::configuration> executor_config_) :
				preprocessor_config(std::move(preprocessor_config_)), 
				vectorizer_config(std::move(vectorizer_config_)),
				interpreter_config(std::move(interpreter_config_)),
				modeller_config(std::move(modeller_config_)),
				executor_config(std::move(executor_config_))
		{}

		std::unique_ptr<preprocessor::preprocessor> configured_preprocessor() const
		{
			return std::make_unique<preprocessor::preprocessor>(*preprocessor_config);
		}

		std::unique_ptr<vectorizer::vectorizer> configured_vectorizer() const
		{
			return std::make_unique<vectorizer::vectorizer>(vectorizer_config);
		}

		std::unique_ptr<interpreter::interpreter> configured_interpreter() const
		{
			return std::make_unique<interpreter::interpreter>(*interpreter_config);
		}

		std::unique_ptr<modeller::modeller> configured_modeller() const
		{
			return std::make_unique<modeller::modeller>(*modeller_config);
		}

		std::unique_ptr<preprocessor::input> fetch_input()
		{
			return std::unique_ptr<preprocessor::input>();
		}

		int release_output(std::unique_ptr<modeller::output> output)
		{
			return 0;
		}

		impl operator=(const impl&) = delete;
		impl(const impl&) = delete;

	private:
		std::shared_ptr<preprocessor::configuration> preprocessor_config;
		std::shared_ptr<vectorizer::configuration> vectorizer_config;
		std::shared_ptr<interpreter::configuration> interpreter_config;
		std::shared_ptr<modeller::configuration> modeller_config;
		std::shared_ptr<executor::configuration> executor_config;
	};

	whole_application::whole_application(std::shared_ptr<preprocessor::configuration> preprocessor_config, std::shared_ptr<vectorizer::configuration> vectorizer_config,
		std::shared_ptr<interpreter::configuration> interpreter_config, std::shared_ptr<modeller::configuration> modeller_config, 
		std::shared_ptr<executor::configuration> executor_config) :
		pimpl(std::make_unique<impl>(std::move(preprocessor_config), 
			std::move(vectorizer_config),
			std::move(interpreter_config),
			std::move(modeller_config),
			std::move(executor_config)))
	{}

	whole_application::~whole_application()
	{}

	int whole_application::run()
	{
		processor p(pimpl->configured_preprocessor(), pimpl->configured_vectorizer(), pimpl->configured_interpreter(), pimpl->configured_modeller());

		return pimpl->release_output(p.process(pimpl->fetch_input()));
	}
}
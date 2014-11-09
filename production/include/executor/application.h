#ifndef APPLICATION_H_JFEWUINDVJKNfuifnewufjiwefjoiwekdsmfkjnedgivnerigovrefefdwedwecvdfgvrthbtynbtynytythbythyt
#define APPLICATION_H_JFEWUINDVJKNfuifnewufjiwefjoiwekdsmfkjnedgivnerigovrefefdwedwecvdfgvrthbtynbtynytythbythyt

#include <memory>

namespace preprocessor
{
	class configuration;
}

namespace vectorizer
{
	class configuration;
}

namespace interpreter
{
	class configuration;
}

namespace modeller
{
	class configuration;
}

namespace executor
{
	class configuration;
}

namespace executor
{
	class application
	{
	public:
		virtual ~application(){}
		virtual int run() = 0;
	};

	class whole_application : public application
	{
		class impl;
	public:
		whole_application(std::shared_ptr<preprocessor::configuration>, std::shared_ptr<vectorizer::configuration>, 
			std::shared_ptr<interpreter::configuration>, std::shared_ptr<modeller::configuration>, std::shared_ptr<::executor::configuration>);
		~whole_application();

		int run();

	private:
		std::unique_ptr<impl> pimpl;
	};
}

#endif
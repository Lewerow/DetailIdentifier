#include <executor/processor.h>

#include <preprocessor/preprocessor.h>
#include <vectorizer/vectorizer.h>
#include <interpreter/interpreter.h>
#include <modeller/modeller.h>


namespace executor
{
	processor::processor(std::unique_ptr<preprocessor::preprocessor> prep, std::unique_ptr<vectorizer::vectorizer> vec, std::unique_ptr<interpreter::interpreter> interp, std::unique_ptr<modeller::modeller> model) : 
		preprocessor(std::move(prep)), vectorizer(std::move(vec)), interpreter(std::move(interp)), modeller(std::move(model))
	{}

	std::unique_ptr<modeller::output> processor::process(std::unique_ptr<preprocessor::input> input)
	{
		std::unique_ptr<preprocessor::output> preprocessed_data = preprocessor->preprocess(std::move(input));
		std::unique_ptr<vectorizer::output> vectorized_data = vectorizer->vectorize(std::move(preprocessed_data));
		std::unique_ptr<interpreter::output> interpreted_data = interpreter->generate_interpretation(std::move(vectorized_data));
		std::unique_ptr<modeller::output> model = modeller->generate_model(std::move(interpreted_data));

		return model;
	}
}
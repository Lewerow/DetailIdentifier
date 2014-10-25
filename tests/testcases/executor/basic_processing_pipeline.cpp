#include <boost/test/auto_unit_test.hpp>
#include <turtle/mock.hpp>

#include <functional>

#include <executor/processor.h>

#include <doubles/preprocessor_mock.h>
#include <doubles/vectorizer_mock.h>
#include <doubles/interpreter_mock.h>
#include <doubles/modeller_mock.h>

#include <helpers/unique_ptr_verifiers.h>

BOOST_AUTO_TEST_SUITE(executor_tests)

BOOST_AUTO_TEST_CASE(basic_processing_pipeline)
{
	std::unique_ptr<mocks::preprocessor> preprocessor_mock(std::make_unique<mocks::preprocessor>());
	std::unique_ptr<mocks::vectorizer> vectorizer_mock(std::make_unique<mocks::vectorizer>());
	std::unique_ptr<mocks::interpreter> interpreter_mock(std::make_unique<mocks::interpreter>());
	std::unique_ptr<mocks::modeller> modeller_mock(std::make_unique<mocks::modeller>());

	MOCK_EXPECT(preprocessor_mock->preprocess).once().with(nullptr).moves(nullptr);
	MOCK_EXPECT(vectorizer_mock->vectorize).once().with(nullptr).moves(nullptr);
	MOCK_EXPECT(interpreter_mock->generate_interpretation).once().with(nullptr).moves(nullptr);
	MOCK_EXPECT(modeller_mock->generate_model).once().with(nullptr).moves(nullptr);
	executor::processor proc(std::move(preprocessor_mock), std::move(vectorizer_mock), std::move(interpreter_mock), std::move(modeller_mock));

	BOOST_CHECK(proc.process(nullptr).get() == nullptr);
}

BOOST_AUTO_TEST_CASE(proper_data_is_passed_in_basic_processing_pipeline)
{
	std::unique_ptr<mocks::preprocessor> preprocessor_mock(std::make_unique<mocks::preprocessor>());
	std::unique_ptr<mocks::vectorizer> vectorizer_mock(std::make_unique<mocks::vectorizer>());
	std::unique_ptr<mocks::interpreter> interpreter_mock(std::make_unique<mocks::interpreter>());
	std::unique_ptr<mocks::modeller> modeller_mock(std::make_unique<mocks::modeller>());

	std::unique_ptr<preprocessor::input> preprocessor_in = std::make_unique<preprocessor::input>();
	std::unique_ptr<vectorizer::input> vectorizer_in = std::make_unique<vectorizer::input>();
	std::unique_ptr<interpreter::input> interpreter_in = std::make_unique<interpreter::input>();
	std::unique_ptr<modeller::input> modeller_in = std::make_unique<modeller::input>();
	std::unique_ptr<modeller::output> modeller_out = std::make_unique<modeller::output>();
	modeller::output* out_val = modeller_out.get();
	
	MOCK_EXPECT(modeller_mock->generate_model).once().with(mock::call(helpers::exact_unique(modeller_in.get()))).moves(std::move(modeller_out));
	MOCK_EXPECT(interpreter_mock->generate_interpretation).once().with(mock::call(helpers::exact_unique(interpreter_in.get()))).moves(std::move(modeller_in));
	MOCK_EXPECT(vectorizer_mock->vectorize).once().with(mock::call(helpers::exact_unique(vectorizer_in.get()))).moves(std::move(interpreter_in));
	MOCK_EXPECT(preprocessor_mock->preprocess).once().with(mock::call(helpers::exact_unique(preprocessor_in.get()))).moves(std::move(vectorizer_in));
	
	executor::processor proc(std::move(preprocessor_mock), std::move(vectorizer_mock), std::move(interpreter_mock), std::move(modeller_mock));

	BOOST_CHECK_EQUAL(out_val, proc.process(std::move(preprocessor_in)).get());
}

BOOST_AUTO_TEST_SUITE_END()
#include <boost/test/auto_unit_test.hpp>
#include <turtle/mock.hpp>

#include <executor/processor.h>

#include <doubles/preprocessor_mock.h>
#include <doubles/vectorizer_mock.h>
#include <doubles/interpreter_mock.h>
#include <doubles/modeller_mock.h>

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

BOOST_AUTO_TEST_SUITE_END()
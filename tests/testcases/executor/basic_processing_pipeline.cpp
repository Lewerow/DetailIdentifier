#include <boost/test/auto_unit_test.hpp>
#include <turtle/mock.hpp>

#include <executor/processor.h>

#include <doubles/preprocessor_mock.h>

BOOST_AUTO_TEST_SUITE(executor_tests)

BOOST_AUTO_TEST_CASE(basic_processing_pipeline)
{
	std::unique_ptr<mocks::preprocessor> preprocessor_mock;

	MOCK_EXPECT(preprocessor_mock->preprocess).with(nullptr).moves(nullptr);
	executor::processor proc(std::make_unique<mocks::preprocessor>());

	proc.process(nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
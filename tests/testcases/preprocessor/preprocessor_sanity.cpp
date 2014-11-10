#include <boost/test/auto_unit_test.hpp>

#include <boost/filesystem/operations.hpp>

#include <doubles/logger_mock.h>
#include <helpers/simple_configuration.h>

#include <preprocessor/configuration.h>
#include <preprocessor/preprocessor.h>

BOOST_AUTO_TEST_SUITE(preprocessor_tests)

BOOST_AUTO_TEST_CASE(basic_sanity)
{
	BOOST_REQUIRE(boost::filesystem::exists(TEST_FILES_DIRECTORY_STRING + "preprocessor/in_sanity.jpg"));

	mocks::logger log;
	preprocessor::configuration config(boost::program_options::variables_map{}, log);
	config.set_workspace(TEST_FILES_DIRECTORY_STRING + "workspace");
	config.set_input_filename(TEST_FILES_DIRECTORY_STRING + "preprocessor/in_sanity.jpg");

	preprocessor::preprocessor p(config);
	auto out = p.preprocess(std::unique_ptr<preprocessor::input>());

	BOOST_CHECK(boost::filesystem::exists(TEST_FILES_DIRECTORY_STRING + "workspace/preprocessed.pgm"));
	BOOST_CHECK_EQUAL(TEST_FILES_DIRECTORY_STRING + "workspace/preprocessed.pgm", out->filename);
}

BOOST_AUTO_TEST_SUITE_END()
#include <boost/test/auto_unit_test.hpp>

#include <boost/filesystem/operations.hpp>

#include <doubles/logger_mock.h>
#include <helpers/simple_configuration.h>

#include <preprocessor/configuration.h>
#include <preprocessor/preprocessor.h>

BOOST_AUTO_TEST_SUITE(preprocessor_tests)

BOOST_AUTO_TEST_CASE(basic_sanity)
{
	const std::string input_path = TEST_FILES_DIRECTORY_STRING + "preprocessor/in_sanity.jpg";
	const std::string output_path = TEST_FILES_DIRECTORY_STRING + "workspace/preprocessed.pgm";

	BOOST_REQUIRE(boost::filesystem::exists(input_path));
	if (boost::filesystem::exists(output_path))
		boost::filesystem::remove(output_path);

	BOOST_REQUIRE(!boost::filesystem::exists(output_path));
	
	mocks::logger log;
	preprocessor::configuration config(boost::program_options::variables_map{}, log);
	config.set_workspace(TEST_FILES_DIRECTORY_STRING + "workspace");
	config.set_input_filename(input_path);

	preprocessor::preprocessor p(config);
	auto out = p.preprocess(std::unique_ptr<preprocessor::input>());

	BOOST_CHECK(boost::filesystem::exists(out->filename));
}

BOOST_AUTO_TEST_SUITE_END()
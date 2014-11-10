#include <boost/test/auto_unit_test.hpp>

#include <boost/filesystem/operations.hpp>

#include <boost/program_options/variables_map.hpp>

#include <executor/exceptions.h>
#include <vectorizer/vectorizer.h>
#include <vectorizer/configuration.h>

#include <doubles/os_proxy_mock.h>
#include <doubles/logger_mock.h>

#include <functional>

namespace
{
	bool args_comparator(const std::string& in, const std::string& out, const std::string& arg, const std::vector<std::string>& args)
	{
		auto arg_exists = std::find(args.begin(), args.end(), arg);
		if (arg_exists == args.end())
			return false;

		auto out_exists = std::find(arg_exists, args.end(), out);
		if (out_exists == args.end() || !(*(out_exists - 1) == "-o" || *(out_exists - 1) == "--output"))
			return false;

		auto in_exists = std::find(out_exists, args.end(), in);
		if (out_exists == args.end() || *(in_exists - 1) != "--")
			return false;

		return true;
	}

	const std::string input_path = TEST_FILES_DIRECTORY_STRING + "vectorizer/in_sanity.pgm";
	const std::string output_path_svg = TEST_FILES_DIRECTORY_STRING + "workspace/vectorized.svg";
	const std::string output_path_dxf = TEST_FILES_DIRECTORY_STRING + "workspace/vectorized.dxf";
	const std::string output_path_pgm = TEST_FILES_DIRECTORY_STRING + "workspace/vectorization_input.pgm";

	struct fixture
	{
		fixture() : os(std::make_shared<mocks::os_proxy>()), config(std::make_shared<vectorizer::configuration>(boost::program_options::variables_map{}, log, os))
		{
			config->set_workspace(TEST_FILES_DIRECTORY_STRING + "workspace");
			config->set_potrace_location(DEFAULT_POTRACE_EXECUTABLE_NAME_STRING);
			config->set_mkbitmap_location(DEFAULT_MKBITMAP_EXECUTABLE_NAME_STRING);

			BOOST_REQUIRE(boost::filesystem::exists(input_path));
			if (boost::filesystem::exists(output_path_svg))
				boost::filesystem::remove(output_path_svg);

			if (boost::filesystem::exists(output_path_dxf))
				boost::filesystem::remove(output_path_dxf);

			if (boost::filesystem::exists(output_path_pgm))
				boost::filesystem::remove(output_path_pgm);
		}


		mocks::logger log;
		std::shared_ptr<mocks::os_proxy> os;
		std::shared_ptr<vectorizer::configuration> config;
	};
}

BOOST_FIXTURE_TEST_SUITE(vectorizer_tests, fixture)

BOOST_AUTO_TEST_CASE(basic_sanity_ut)
{
	vectorizer::vectorizer v(config);

	std::unique_ptr<vectorizer::input> in = std::make_unique<vectorizer::input>();
	in->filename = input_path;

	MOCK_EXPECT(os->call).once().with(DEFAULT_POTRACE_EXECUTABLE_NAME_STRING, std::bind(args_comparator, input_path, output_path_svg, "--svg", std::placeholders::_1));
	MOCK_EXPECT(os->call).once().with(DEFAULT_POTRACE_EXECUTABLE_NAME_STRING, std::bind(args_comparator, input_path, output_path_dxf, "dxf", std::placeholders::_1));
	MOCK_EXPECT(os->copy).once().with(input_path, output_path_pgm);

	auto out = v.vectorize(std::move(in));
}

BOOST_AUTO_TEST_CASE(throws_on_failed_syscall)
{
	vectorizer::vectorizer v(config);

	std::unique_ptr<vectorizer::input> in = std::make_unique<vectorizer::input>();
	in->filename = input_path;


	MOCK_EXPECT(log.error_log).once();
	MOCK_EXPECT(os->call).once().with(DEFAULT_POTRACE_EXECUTABLE_NAME_STRING, std::bind(args_comparator, input_path, output_path_svg, "--svg", std::placeholders::_1)).throws(executor::system_call_failure(""));

	BOOST_CHECK_THROW(v.vectorize(std::move(in)), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(experimental_MT)
{
	config->set_os_proxy(std::make_shared<executor::os_proxy>());
	vectorizer::vectorizer v(config);

	std::unique_ptr<vectorizer::input> in = std::make_unique<vectorizer::input>();
	in->filename = input_path;

	auto out = v.vectorize(std::move(in));

	BOOST_CHECK(boost::filesystem::exists(out->pgm_input_filename));
	BOOST_CHECK(boost::filesystem::exists(out->svg_filename));
	BOOST_CHECK(boost::filesystem::exists(out->dxf_filename));
}
BOOST_AUTO_TEST_SUITE_END()
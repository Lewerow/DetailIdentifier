#ifndef EXECUTOR_H_mdiewfm49fmuifmvrekmoiewmfioemfiemfi34emf
#define EXECUTOR_H_mdiewfm49fmuifmvrekmoiewmfioemfiemfi34emf

#include <boost/program_options.hpp>

namespace executor
{
    class executor
    {
    public:
        int execute(int argc, char** argv);

		static boost::program_options::options_description all_application_options();
    };
}

#endif

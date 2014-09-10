#include "tests/integration/test_header.hpp"
#include "tests/integration/x86/ring_IClamp/main.h"

#include "tests/integration/x86/ring_IClamp/ring_ref_solution.h"

using namespace corebluron::test;

BOOST_AUTO_TEST_CASE(ring_validation)
{
    std::string path("../../../../tests/integration/x86/ring_IClamp/");
    reference ref(path);
    std::vector<std::pair<double, int> > res;
    char* env[256];
    (void)main_ring(1, boost::unit_test::framework::master_test_suite().argv, &env[0], path); 
    validation(res);

    BOOST_CHECK_EQUAL(ref.size(), res.size());

    for(size_t i=0; i < res.size(); ++i)
        BOOST_CHECK_CLOSE(res[i].first, ref[i].first, 0.001 );

}

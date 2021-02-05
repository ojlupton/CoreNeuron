#define BOOST_TEST_MODULE LFPTest
#define BOOST_TEST_MAIN

#include <iostream>
#include <mpi.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include "coreneuron/io/lfp.hpp"

template <typename F>
double integral(F f, double a, double b, int n) {
    double step = (b - a) / n;  // width of each small rectangle
    double area = 0.0;  // signed area
    for (int i = 0; i < n; i ++) {
        area += f(a + (i + 0.5) * step) * step; // sum up each small rectangle
    }
    return area;
}

std::array<double,3> operator * (const double x, const std::array<double,3>& y) {
	return {x * y[0], x * y[1], x * y[2]};
}

std::array<double,3> operator + (const std::array<double,3>& x, const std::array<double,3>& y) {
        return {x[0] + y[0], x[1] + y[1], x[2] + y[2]};
}

BOOST_AUTO_TEST_CASE(LFP_PointSource_LineSource) {
    MPI_Init(nullptr, nullptr);
    using namespace coreneuron;
    double segment_length{1.0e-6};
    double segment_start{1.0e-6};
    std::array<double, 3> segment_start = {0.0, 0.0, segment_start};
    std::array<double, 3> segment_end = {0.0, 0.0, segment_start + segment_length};
    double floor{1.0e-6};
    double pi{3.141592653589};

    std::array<double, 10> vals;
    double circling_radius{1.0e-6};
    std::array<double,3> segment_middle{0.0, 0.0, 1.5e-6};
    double medium_resistivity_fac{1.0};
    for (auto k = 0; k < 10; k++) {
        std::array<double, 3> approaching_elec = segment_middle + 
               {0.0, 1.0e-5 - k * 1.0e-6, 0.0};
        std::array<double, 3> circling_elec = segment_middle + 
               {0.0, circling_radius * std::cos(2.0 * pi * k / 10),
                          circling_radius * std::sin(2.0 * pi * k / 10)};

        double l = line_source_lfp_factor(
                approaching_elec,
                segment_start,
                segment_end,
                floor,
                medium_resistivity_fac);
        double l2 = line_source_lfp_factor(
                circling_elec,
                segment_start,
                segment_end,
                floor,
                medium_resistivity_fac);
        double l3 = integral ([&](double x) {
		return 1.0 / std::max(floor, 
               norm<std::array<double,3>,double>(
                   circling_elec 
                   + (-x) * segment_start 
                   + (x - 1) * segment_end));
	        }, 0.0, 1.0, 10000);
        // TEST of analytic vs numerical integration
        BOOST_REQUIRE_CLOSE(l2, l3, 1.0e-6);
        // TEST of LFP Flooring
        BOOST_REQUIRE(e[1] < 0.866e-6 || l != 1.0e6);
        vals[k] = l2;
    }
    // TEST of SYMMETRY of LFP FORMULA
    for (size_t k = 0; k < 5; k++) {
        BOOST_REQUIRE(std::abs((vals[k] - vals[k + 5]) * 0.5 / (vals[k] + vals[k + 5]))
            < 1.0e-12);
    }
    std::vector<std::array<double, 3> > start = {{0.,  0.,  1.},
                                                 {0.,  0.,  0.5},
                                                 {0.0, 0.0, 0.0},
                                                 {0.0, 0.0, -0.5}};
    std::vector<std::array<double, 3> > end = {{0.,  0.,  0.},
                                               {0.,  0.,  1.},
                                               {0.,  0.,  0.5},
                                               {0.0, 0.0, 0.0}};
    std::vector<double> radius{0.1, 0.1, 0.1, 0.1};
    std::vector<std::array<double, 3> > elect = {{0.0, 0.3, 0.0},
                                                 {0.0, 0.7, 0.8}};

    std::vector<int> indices = {0, 1, 2, 3};
    LFPCalculator<LineSource> lfp(MPI_COMM_WORLD, start, end, radius, indices, elect, 1.0);
    std::vector<double> res_line_source = lfp.template lfp<std::vector<double>>({0.0, 1.0, 2.0, 3.0});     
    LFPCalculator<PointSource> lfpp(MPI_COMM_WORLD, start, end, radius, indices, elect, 1.0);
    std::vector<double> res_point_source = lfpp.template lfp<std::vector<double>>({0.0, 1.0, 2.0, 3.0});
    BOOST_REQUIRE_CLOSE(res_line_source[0], res_point_source[0], 1.0e-3);
    BOOST_REQUIRE_CLOSE(res_line_source[1], res_point_source[1], 1.0e-3);
    MPI_Finalize();

}


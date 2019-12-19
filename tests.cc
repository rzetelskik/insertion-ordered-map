#include "insertion_ordered_map.h"

#define BOOST_TEST_MODULE tests
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

struct cout_redirect {
    cout_redirect(std::streambuf * new_buffer) : old( std::cout.rdbuf(new_buffer)) {}

    ~cout_redirect() {
        std::cout.rdbuf(old);
    }

private:
    std::streambuf * old;
};

BOOST_AUTO_TEST_CASE(insert) {
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(!aaa.insert(4, 5) && (aaa.size() == 1));
}

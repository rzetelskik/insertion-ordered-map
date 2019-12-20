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

BOOST_AUTO_TEST_CASE(erase) {
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK_NO_THROW(aaa.erase(4));
    BOOST_CHECK(aaa.empty());
    BOOST_CHECK_THROW(aaa.erase(4), lookup_error);
    BOOST_CHECK_THROW(aaa.erase(5), lookup_error);
}

BOOST_AUTO_TEST_CASE(contains) {
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(aaa.contains(4));
    BOOST_CHECK(!aaa.contains(5));
}

BOOST_AUTO_TEST_CASE(copy_constructor) {
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(aaa.insert(5, 5) && (aaa.size() == 2));
    insertion_ordered_map<int, int> bbb(aaa);
    BOOST_CHECK(bbb.contains(4));
    BOOST_CHECK(bbb.contains(5));
}

BOOST_AUTO_TEST_CASE(at) {
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(aaa.insert(5, 5) && (aaa.size() == 2));
    BOOST_CHECK(aaa.at(4) == 5);
    BOOST_CHECK(aaa.at(5) == 5);
}

BOOST_AUTO_TEST_SUITE(copy_on_write)
    BOOST_AUTO_TEST_CASE(basic) {
        insertion_ordered_map<int, int> aaa;
        BOOST_CHECK(aaa.insert(4, 5));
        BOOST_CHECK(aaa.insert(6, 5));
        BOOST_CHECK(aaa.size() == 2);
        insertion_ordered_map<int, int> bbb(aaa);
        insertion_ordered_map<int, int> ccc(aaa);
        BOOST_CHECK(aaa.insert(5, 1));
        BOOST_CHECK(aaa.contains(5));
        BOOST_CHECK(!bbb.contains(5));
        BOOST_CHECK(!ccc.contains(5));
    }

    BOOST_AUTO_TEST_CASE(list_iter_update) {
        insertion_ordered_map<int, int> aaa;
        BOOST_CHECK_NO_THROW(aaa.insert(4, 5));
        BOOST_CHECK_NO_THROW(aaa.insert(6, 5));
        BOOST_CHECK(aaa.at(4) == 5);
        BOOST_CHECK(aaa.at(6) == 5);
        insertion_ordered_map<int, int> bbb(aaa);
        aaa.clear();
        BOOST_CHECK_NO_THROW(bbb.at(4));
        BOOST_CHECK_NO_THROW(bbb.at(6));
        BOOST_CHECK_THROW(aaa.at(4), lookup_error);
        BOOST_CHECK_THROW(aaa.at(6), lookup_error);
    }

BOOST_AUTO_TEST_SUITE_END()


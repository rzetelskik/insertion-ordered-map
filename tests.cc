#include "insertion_ordered_map.h"

#define BOOST_TEST_MODULE tests

#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

struct cout_redirect
{
    cout_redirect(std::streambuf *new_buffer) : old(std::cout.rdbuf(new_buffer))
    {}

    ~cout_redirect()
    {
        std::cout.rdbuf(old);
    }

private:
    std::streambuf *old;
};

BOOST_AUTO_TEST_CASE(insert)
{
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(!aaa.insert(4, 5) && (aaa.size() == 1));
}

BOOST_AUTO_TEST_CASE(erase)
{
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK_NO_THROW(aaa.erase(4));
    BOOST_CHECK(aaa.empty());
    BOOST_CHECK_THROW(aaa.erase(4), lookup_error);
    BOOST_CHECK_THROW(aaa.erase(5), lookup_error);
}

BOOST_AUTO_TEST_CASE(contains)
{
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(aaa.contains(4));
    BOOST_CHECK(!aaa.contains(5));
}

BOOST_AUTO_TEST_CASE(copy_constructor)
{
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(aaa.insert(5, 5) && (aaa.size() == 2));
    insertion_ordered_map<int, int> bbb(aaa);
    BOOST_CHECK(bbb.contains(4));
    BOOST_CHECK(bbb.contains(5));
}

BOOST_AUTO_TEST_CASE(copy_on_write)
{
    insertion_ordered_map<int, int> aaa;
    BOOST_CHECK(aaa.insert(4, 5) && (aaa.size() == 1));
    BOOST_CHECK(aaa.insert(6, 5) && (aaa.size() == 2));
    insertion_ordered_map<int, int> bbb(aaa);
    insertion_ordered_map<int, int> ccc(aaa);
    BOOST_CHECK(aaa.insert(5, 5) && (aaa.size() == 3));
    BOOST_CHECK(!bbb.contains(5));
    BOOST_CHECK(!ccc.contains(5));
}

BOOST_AUTO_TEST_CASE(iterator)
{
    insertion_ordered_map<int, int> iom;
    insertion_ordered_map<int, int>::iterator it1;

    BOOST_ASSERT(iom.insert(1, 2));
    BOOST_ASSERT(iom.size() == 1);
    BOOST_ASSERT(iom.insert(2, 3));
    BOOST_ASSERT(iom.size() == 2);
    BOOST_ASSERT(iom.insert(3, 4));
    BOOST_ASSERT(iom.size() == 3);

    insertion_ordered_map<int, int>::iterator it2(iom.begin());

    insertion_ordered_map<int, int>::iterator it3(it1);

    it3 = iom.end();

    BOOST_ASSERT(it2->first == 1);
    BOOST_ASSERT(it2->second == 2);

    ++it2;
    BOOST_ASSERT(it2->first == 2);
    BOOST_ASSERT((*it2).second == 3);

    it1 = it2;

    ++it2;

    BOOST_ASSERT((*it1).first == 2);
    BOOST_ASSERT((*it1).second == 3);

    BOOST_ASSERT((*it2).first == 3);
    BOOST_ASSERT((*it2).second == 4);

    ++it2;
    BOOST_ASSERT(it2 == iom.end());
}
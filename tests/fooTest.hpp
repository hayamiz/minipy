// -*- coding: utf-8 -*-

#ifndef footest_HPP
#define footest_HPP

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include "./../foo.hpp"
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

/* テストクラスは TestCase を継承する。 */
class fooTest : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( fooTest );

    CPPUNIT_TEST( test_case1 );

    CPPUNIT_TEST_SUITE_END();

    // fixtures
int qux;


public:
    /* テストは test_XXX という名前にする。 */

    void test_case1();

};

#endif

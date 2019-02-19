/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   line_test.h
 * Author: tylerw
 *
 * Created on Feb 18, 2019, 8:04:43 PM
 */

#ifndef LINE_TEST_H
#define LINE_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class line_test : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(line_test);

    CPPUNIT_TEST(testMethod);
    CPPUNIT_TEST(testFailedMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    line_test();
    virtual ~line_test();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif /* LINE_TEST_H */


/*=============================================================================
    Copyright (c) 2001-2006 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include <vector>
#include <string>
#include <map>
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/phoenix/core/argument.hpp>
#include <boost/spirit/phoenix/core/reference.hpp>
#include <boost/spirit/phoenix/operator/self.hpp>

using namespace boost::phoenix;
using namespace std;

int
main()
{
    {
        int x = 123;
        BOOST_TEST((&arg1)(x) == &x);
        BOOST_TEST((*&arg1)(x) == 123);

        int y = 968;
        (ref(x) = arg1)(y);
        BOOST_TEST(x == y);

        (arg1 = 456)(x);
        BOOST_TEST(x == 456);
        int& r = (arg1 = 456)(x); // must be an lvalue
        BOOST_TEST(&r == &x);

        int c[] = { 1, 2, 3, 4, 5 };
        BOOST_TEST((arg1[3])(c) == 4);

        int& r2 = (arg1[3])(c); // must be an lvalue
        BOOST_TEST(&r2 == &c[3]);

        vector<string> v;
        v.push_back("a");
        v.push_back("b");
        v.push_back("c");
        v.push_back("d");

        BOOST_TEST((arg1[3])(v) == "d");

        map<string, int> m;
        (arg1["Kimpo"] = arg2)(m, x);
        BOOST_TEST(m["Kimpo"] == x);
    }

    return boost::report_errors();
}

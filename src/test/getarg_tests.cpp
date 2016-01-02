#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-LILY");
    BOOST_CHECK(GetBoolArg("-LILY"));
    BOOST_CHECK(GetBoolArg("-LILY", false));
    BOOST_CHECK(GetBoolArg("-LILY", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BARo"));
    BOOST_CHECK(!GetBoolArg("-BARo", false));
    BOOST_CHECK(GetBoolArg("-BARo", true));

    ResetArgs("-LILY=0");
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", false));
    BOOST_CHECK(!GetBoolArg("-LILY", true));

    ResetArgs("-LILY=1");
    BOOST_CHECK(GetBoolArg("-LILY"));
    BOOST_CHECK(GetBoolArg("-LILY", false));
    BOOST_CHECK(GetBoolArg("-LILY", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", false));
    BOOST_CHECK(!GetBoolArg("-LILY", true));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", false));
    BOOST_CHECK(!GetBoolArg("-LILY", true));

    ResetArgs("-LILY -noBAR");  // -LILY should win
    BOOST_CHECK(GetBoolArg("-LILY"));
    BOOST_CHECK(GetBoolArg("-LILY", false));
    BOOST_CHECK(GetBoolArg("-LILY", true));

    ResetArgs("-LILY=1 -noBAR=1");  // -LILY should win
    BOOST_CHECK(GetBoolArg("-LILY"));
    BOOST_CHECK(GetBoolArg("-LILY", false));
    BOOST_CHECK(GetBoolArg("-LILY", true));

    ResetArgs("-LILY=0 -noBAR=0");  // -LILY should win
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", false));
    BOOST_CHECK(!GetBoolArg("-LILY", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--LILY=1");
    BOOST_CHECK(GetBoolArg("-LILY"));
    BOOST_CHECK(GetBoolArg("-LILY", false));
    BOOST_CHECK(GetBoolArg("-LILY", true));

    ResetArgs("--noBAR=1");
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", false));
    BOOST_CHECK(!GetBoolArg("-LILY", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-LILY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LILY", "eleven"), "eleven");

    ResetArgs("-LILY -bar");
    BOOST_CHECK_EQUAL(GetArg("-LILY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LILY", "eleven"), "");

    ResetArgs("-LILY=");
    BOOST_CHECK_EQUAL(GetArg("-LILY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LILY", "eleven"), "");

    ResetArgs("-LILY=11");
    BOOST_CHECK_EQUAL(GetArg("-LILY", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-LILY", "eleven"), "11");

    ResetArgs("-LILY=eleven");
    BOOST_CHECK_EQUAL(GetArg("-LILY", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-LILY", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-LILY", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-LILY", 0), 0);

    ResetArgs("-LILY -bar");
    BOOST_CHECK_EQUAL(GetArg("-LILY", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-LILY=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-LILY", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-LILY=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-LILY", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--LILY");
    BOOST_CHECK_EQUAL(GetBoolArg("-LILY"), true);

    ResetArgs("--LILY=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-LILY", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBAR");
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", true));
    BOOST_CHECK(!GetBoolArg("-LILY", false));

    ResetArgs("-noBAR=1");
    BOOST_CHECK(!GetBoolArg("-LILY"));
    BOOST_CHECK(!GetBoolArg("-LILY", true));
    BOOST_CHECK(!GetBoolArg("-LILY", false));

    ResetArgs("-noBAR=0");
    BOOST_CHECK(GetBoolArg("-LILY"));
    BOOST_CHECK(GetBoolArg("-LILY", true));
    BOOST_CHECK(GetBoolArg("-LILY", false));

    ResetArgs("-LILY --noBAR");
    BOOST_CHECK(GetBoolArg("-LILY"));

    ResetArgs("-noBAR -LILY"); // LILY always wins:
    BOOST_CHECK(GetBoolArg("-LILY"));
}

BOOST_AUTO_TEST_SUITE_END()

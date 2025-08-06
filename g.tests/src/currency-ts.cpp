#include "utils/currency.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <tuple>

struct CurrencyDescriptor
{
    int64_t minor_units = 0;
    double major_units = 0;
    int precision = 0;
    std::string code;
};

using InitTestParams = CurrencyDescriptor;
using ConvertTestParams = std::tuple<CurrencyDescriptor, double, std::string, int, CurrencyDescriptor>;

class CurrencyInit_TS : public ::testing::TestWithParam<InitTestParams>
{
};

class CurrencyConvert_TS : public ::testing::TestWithParam<ConvertTestParams>
{
};

INSTANTIATE_TEST_SUITE_P(
    Initialization,
    CurrencyInit_TS,
    ::testing::Values(CurrencyDescriptor{2500, 25, 2, "usd"},
                      CurrencyDescriptor{4500, 45, 2, "uah"},
                      CurrencyDescriptor{37, 0.37, 2, "eur"}));

INSTANTIATE_TEST_SUITE_P(
    Conversion,
    CurrencyConvert_TS,
    ::testing::Values(std::make_tuple(CurrencyDescriptor{2500, 25, 2, "usd"},
                                      41.71458864, "uah", 2,
                                      CurrencyDescriptor{104286, 1042.86, 2, "uah"}),
                      std::make_tuple(CurrencyDescriptor{1000, 10, 2, "usd"},
                                      0.86389992, "eur", 2,
                                      CurrencyDescriptor{864, 8.64, 2, "eur"})));

TEST_P(CurrencyInit_TS, Initialization)
{
    const auto& params = GetParam();

    Currency sut(params.minor_units, params.code, params.precision);

    EXPECT_EQ(sut.rawMinorUnits(), params.minor_units);
    EXPECT_EQ(sut.to_double(), params.major_units);
    EXPECT_EQ(sut.get_currency_code(), params.code);
    EXPECT_EQ(sut.get_precision(), params.precision);
}

TEST_P(CurrencyConvert_TS, Conversion)
{
    const auto [src, rate, code_to, precision, dest] = GetParam();
    const Currency from(src.minor_units, src.code, src.precision);

    const Currency target = Currency::convert(from, code_to, rate, precision);

    EXPECT_EQ(target.rawMinorUnits(), dest.minor_units);
    EXPECT_EQ(target.to_double(), dest.major_units);
    EXPECT_EQ(target.get_currency_code(), dest.code);
    EXPECT_EQ(target.get_precision(), dest.precision);
}
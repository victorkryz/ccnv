#pragma once

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

class Currency
{

public:
    Currency() : Currency(static_cast<double>(1l), "usd") {}
    Currency(double amount, std::string code, int prec = 2)
        : currency_code_(std::move(code)), precision_(prec)
    {
        double scale = std::pow(10.0, precision_);
        minor_units_ = static_cast<int64_t>(amount * scale + (amount >= 0 ? 0.5 : -0.5));
    }

    explicit Currency(int64_t units, std::string code, int prec = 2)
        : minor_units_(units), currency_code_(std::move(code)), precision_(prec) {}

    double to_double() const
    {
        return minor_units_ / std::pow(10.0, precision_);
    }

    std::string get_currency_code_() const { return currency_code_; }
    int get_precision_() const { return precision_; }
    int64_t rawMinorUnits() const { return minor_units_; }

    // Arithmetic (same currency only)
    Currency operator+(const Currency& other) const
    {
        checkCompatibility(other);
        return Currency(minor_units_ + other.minor_units_, currency_code_, precision_);
    }

    Currency operator-(const Currency& other) const
    {
        checkCompatibility(other);
        return Currency(minor_units_ - other.minor_units_, currency_code_, precision_);
    }

    Currency& operator+=(const Currency& other)
    {
        checkCompatibility(other);
        minor_units_ += other.minor_units_;
        return *this;
    }

    Currency& operator-=(const Currency& other)
    {
        checkCompatibility(other);
        minor_units_ -= other.minor_units_;
        return *this;
    }

    // Comparison
    bool operator==(const Currency& other) const
    {
        return currency_code_ == other.currency_code_ && minor_units_ == other.minor_units_;
    }

    bool operator!=(const Currency& other) const { return !(*this == other); }
    bool operator<(const Currency& other) const
    {
        checkCompatibility(other);
        return minor_units_ < other.minor_units_;
    }

    bool operator>(const Currency& other) const
    {
        checkCompatibility(other);
        return minor_units_ > other.minor_units_;
    }

    // Output formatting
    friend std::ostream& operator<<(std::ostream& os, const Currency& m)
    {
        os << std::fixed << std::setprecision(m.precision_)
           << m.to_double() << ' ' << m.currency_code_;
        return os;
    }

    // Currency conversion (static rates)
    static Currency convert(const Currency& from, const std::string& to_currency,
                            double rate, int to_precision = 2)
    {
        if (from.currency_code_ == to_currency)
            return from;
        double base_amount = from.to_double();
        double target_amount = base_amount * rate;
        return Currency(target_amount, to_currency, to_precision);
    }

private:
    void checkCompatibility(const Currency& other) const
    {
        if (currency_code_ != other.currency_code_ || precision_ != other.precision_)
        {
            throw std::invalid_argument("Mismatched currency or precision");
        }
    }

private:
    int64_t minor_units_;       // e.g. cents
    std::string currency_code_; // "usd", "eur", etc.
    int precision_;             // e.g. 2 for USD, EUR
};

#pragma once
#include <string>
#include "BigInt.h"

// Fixed-point decimal represented as (scaled, scale) => value = scaled / 10^scale
class BigDecimal {
public:
    BigDecimal();
    explicit BigDecimal(const std::string& s); // supports optional sign and decimal point
    BigDecimal(const BigInt& scaled, int scale);

    // arithmetic
    static BigDecimal sub(const BigDecimal& a, const BigDecimal& b); // a-b
    static BigDecimal mul(const BigDecimal& a, const BigDecimal& b); // a*b

    int sign() const; // -1,0,1
    int compare(const BigDecimal& other) const; // -1,0,1

    const BigInt& scaled() const { return scaledValue; }
    int scale() const { return scale10; }

    std::string toString() const;

private:
    BigInt scaledValue;
    int scale10 = 0;

    static void align(const BigDecimal& a, const BigDecimal& b, BigInt& as, BigInt& bs, int& sc);
};



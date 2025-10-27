#include "BigDecimal.h"
#include <algorithm>

static BigInt pow10int(int e) {
    BigInt r(1);
    BigInt ten(10);
    for (int i = 0; i < e; ++i) r = BigInt::mul(r, ten);
    return r;
}

BigDecimal::BigDecimal() {}

BigDecimal::BigDecimal(const BigInt& scaled, int scale) : scaledValue(scaled), scale10(scale) {}

BigDecimal::BigDecimal(const std::string& s) {
    // parse sign and decimal point
    std::string str = s;
    bool neg = false;
    if (!str.empty() && (str[0] == '+' || str[0] == '-')) { neg = str[0] == '-'; str.erase(str.begin()); }
    size_t dot = str.find('.');
    if (dot == std::string::npos) {
        scaledValue = BigInt(str);
        scale10 = 0;
    } else {
        std::string a = str.substr(0, dot);
        std::string b = str.substr(dot + 1);
        // remove trailing zeros in fractional part to keep scale small
        while (!b.empty() && b.back() == '0') b.pop_back();
        scale10 = (int)b.size();
        std::string together = a + b;
        if (together.empty()) together = "0";
        scaledValue = BigInt(together);
    }
    if (neg && !scaledValue.isZero()) {
        // flip sign by prefixing minus via add/sub trick
        BigInt zero(0);
        scaledValue = BigInt::sub(zero, scaledValue);
    }
}

void BigDecimal::align(const BigDecimal& a, const BigDecimal& b, BigInt& as, BigInt& bs, int& sc) {
    if (a.scale10 == b.scale10) { as = a.scaledValue; bs = b.scaledValue; sc = a.scale10; return; }
    if (a.scale10 > b.scale10) {
        int diff = a.scale10 - b.scale10;
        bs = BigInt::mul(b.scaledValue, pow10int(diff));
        as = a.scaledValue; sc = a.scale10;
    } else {
        int diff = b.scale10 - a.scale10;
        as = BigInt::mul(a.scaledValue, pow10int(diff));
        bs = b.scaledValue; sc = b.scale10;
    }
}

BigDecimal BigDecimal::sub(const BigDecimal& a, const BigDecimal& b) {
    BigInt as, bs; int sc;
    align(a, b, as, bs, sc);
    BigInt diff = BigInt::sub(as, bs);
    return BigDecimal(diff, sc);
}

BigDecimal BigDecimal::mul(const BigDecimal& a, const BigDecimal& b) {
    BigInt prod = BigInt::mul(a.scaledValue, b.scaledValue);
    return BigDecimal(prod, a.scale10 + b.scale10);
}

int BigDecimal::sign() const { return scaledValue.isZero() ? 0 : (scaledValue.compare(BigInt(0)) < 0 ? -1 : 1); }

int BigDecimal::compare(const BigDecimal& other) const {
    BigInt as, bs; int sc; align(*this, other, as, bs, sc);
    return as.compare(bs);
}

std::string BigDecimal::toString() const {
    // produce decimal string
    BigInt s = scaledValue;
    bool neg = s.compare(BigInt(0)) < 0;
    if (neg) s = BigInt::sub(BigInt(0), s);
    std::string raw = s.toString();
    if (scale10 == 0) return (neg ? std::string("-") : std::string("")) + raw;
    if ((int)raw.size() <= scale10) {
        std::string zeros(scale10 - (int)raw.size() + 1, '0');
        std::string val = std::string("0.") + zeros + raw;
        return (neg ? std::string("-") : std::string("")) + val;
    }
    std::string val = raw.substr(0, raw.size() - scale10) + "." + raw.substr(raw.size() - scale10);
    return (neg ? std::string("-") : std::string("")) + val;
}



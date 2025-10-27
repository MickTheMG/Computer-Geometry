#include "BigInt.h"
#include <cctype>
#include <stdexcept>
#include <algorithm>

BigInt::BigInt() {}

BigInt::BigInt(long long v) {
    if (v == 0) return;
    if (v < 0) { neg = true; v = -v; }
    while (v) { digits.push_back(static_cast<int>(v % BASE)); v /= BASE; }
}

BigInt::BigInt(const std::string& s) {
    std::string str = s;
    if (str.empty()) return;
    if (str[0] == '+') str.erase(str.begin());
    else if (str[0] == '-') { neg = true; str.erase(str.begin()); }
    // remove leading zeros
    size_t p = 0; while (p < str.size() && str[p] == '0') ++p; str.erase(0, p);
    if (str.empty()) { neg = false; return; }
    for (int i = (int)str.size(); i > 0; i -= 9) {
        int start = std::max(0, i - 9);
        int len = i - start;
        int chunk = std::stoi(str.substr(start, len));
        digits.push_back(chunk);
    }
    trim();
}

void BigInt::trim() {
    while (!digits.empty() && digits.back() == 0) digits.pop_back();
    if (digits.empty()) neg = false;
}

int BigInt::absCompare(const BigInt& a, const BigInt& b) {
    if (a.digits.size() != b.digits.size()) return a.digits.size() < b.digits.size() ? -1 : 1;
    for (int i = (int)a.digits.size() - 1; i >= 0; --i) if (a.digits[i] != b.digits[i]) return a.digits[i] < b.digits[i] ? -1 : 1;
    return 0;
}

int BigInt::compare(const BigInt& other) const {
    if (neg != other.neg) return neg ? -1 : 1;
    int c = absCompare(*this, other);
    return neg ? -c : c;
}

BigInt BigInt::absAdd(const BigInt& a, const BigInt& b) {
    BigInt r; r.digits.resize(std::max(a.digits.size(), b.digits.size()) + 1, 0);
    long long carry = 0; size_t i = 0;
    for (; i < r.digits.size(); ++i) {
        long long av = i < a.digits.size() ? a.digits[i] : 0;
        long long bv = i < b.digits.size() ? b.digits[i] : 0;
        long long sum = av + bv + carry;
        r.digits[i] = (int)(sum % BASE);
        carry = sum / BASE;
    }
    r.trim(); return r;
}

BigInt BigInt::absSub(const BigInt& a, const BigInt& b) {
    // assumes |a| >= |b|
    BigInt r; r.digits.resize(a.digits.size(), 0);
    long long carry = 0;
    for (size_t i = 0; i < a.digits.size(); ++i) {
        long long av = a.digits[i];
        long long bv = i < b.digits.size() ? b.digits[i] : 0;
        long long cur = av - bv - carry;
        if (cur < 0) { cur += BASE; carry = 1; } else carry = 0;
        r.digits[i] = (int)cur;
    }
    r.trim(); return r;
}

BigInt BigInt::add(const BigInt& a, const BigInt& b) {
    if (a.neg == b.neg) { BigInt r = absAdd(a, b); r.neg = a.neg; r.trim(); return r; }
    int c = absCompare(a, b);
    if (c == 0) return BigInt();
    if (c > 0) { BigInt r = absSub(a, b); r.neg = a.neg; return r; }
    BigInt r = absSub(b, a); r.neg = b.neg; return r;
}

BigInt BigInt::sub(const BigInt& a, const BigInt& b) {
    BigInt nb = b; nb.neg = !nb.neg; return add(a, nb);
}

BigInt BigInt::mul(const BigInt& a, const BigInt& b) {
    if (a.isZero() || b.isZero()) return BigInt();
    BigInt r; r.digits.assign(a.digits.size() + b.digits.size(), 0);
    for (size_t i = 0; i < a.digits.size(); ++i) {
        long long carry = 0;
        for (size_t j = 0; j < b.digits.size() || carry; ++j) {
            long long cur = r.digits[i + j] + (long long)a.digits[i] * (j < b.digits.size() ? b.digits[j] : 0) + carry;
            r.digits[i + j] = (int)(cur % BASE);
            carry = cur / BASE;
        }
    }
    r.neg = a.neg ^ b.neg; r.trim(); return r;
}

std::string BigInt::toString() const {
    if (digits.empty()) return std::string("0");
    std::string s = neg ? std::string("-") : std::string("");
    int n = (int)digits.size();
    s += std::to_string(digits[n-1]);
    char buf[16];
    for (int i = n-2; i >= 0; --i) {
        std::snprintf(buf, sizeof(buf), "%09d", digits[i]);
        s += buf;
    }
    return s;
}

std::ostream& operator<<(std::ostream& os, const BigInt& v) {
    os << v.toString();
    return os;
}



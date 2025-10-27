#pragma once
#include <string>
#include <vector>
#include <iostream>

// Minimal arbitrary-precision signed integer for Task 3
class BigInt {
public:
    BigInt();
    BigInt(long long v);
    explicit BigInt(const std::string& s); // accepts optional leading +/-, digits only

    std::string toString() const;

    int compare(const BigInt& other) const; // -1,0,1
    bool isZero() const { return digits.empty(); }

    static BigInt add(const BigInt& a, const BigInt& b);
    static BigInt sub(const BigInt& a, const BigInt& b); // a-b
    static BigInt mul(const BigInt& a, const BigInt& b);

private:
    // base 1e9 per limb
    static constexpr int BASE = 1000000000;
    std::vector<int> digits; // little-endian
    bool neg = false;

    static int absCompare(const BigInt& a, const BigInt& b);
    static BigInt absAdd(const BigInt& a, const BigInt& b);
    static BigInt absSub(const BigInt& a, const BigInt& b); // assumes |a|>=|b|
    void trim();
};

std::ostream& operator<<(std::ostream& os, const BigInt& v);



#pragma once
#include <cassert>
#include <cstring>
#include <algorithm> // min, max
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


class BigInteger
{
private:
	static const int BASE = 1'000'000'000;
	static const int BASE_DIGITS = 9;

	int sign = 1;
	std::vector<int> digits;

public:
	BigInteger()
	{}
	BigInteger(int64_t v)
	{
		bool fixRequired = false;
		if (v == std::numeric_limits<int64_t>::min()) {
			++v;
			fixRequired = true;
		}
		if (v < 0) {
			sign = -1;
			v = -v;
		}
		while (v != 0) {
			digits.push_back(v % BASE);
			v /= BASE;
		}
		if (fixRequired) {
			++digits[0];
		}
	}
	BigInteger(const BigInteger& rhs)
		: sign(rhs.sign), digits(rhs.digits)
	{}
	BigInteger(BigInteger&& rhs) noexcept
		: sign(rhs.sign), digits(std::move(rhs.digits))
	{}
	BigInteger(const char* s, size_t length)
	{
		assert(s != nullptr);
		if (length == 0) {
			return;
		}
		int start = (s[0] == '-') ? 1 : 0;
		int end = start;
		for (; end < int(length); ++end) {
			if ((s[end] < '0' || s[end] > '9') && s[end] != '\'') {
				break;
			}
		}
		if (start == end || s[start] == '0' || s[start] == '\'') {
			return;
		}
		int factor = 1;
		digits.push_back(0);
		for (int i = end - 1; i >= start; --i) {
			if (s[i] == '\'') {
				continue;
			}
			digits.back() += (s[i] - '0') * factor;
			factor *= 10;
			if (factor == BigInteger::BASE) {
				factor = 1;
				digits.push_back(0);
			}
		}
		sign = (s[0] == '-') ? -1 : 1;
		normalize();
	}

private:
	BigInteger(const BigInteger& rhs, int from, int to)
		: sign(rhs.sign), digits(rhs.digits.begin() + from, rhs.digits.begin() + to)
	{}

public:
	BigInteger& operator=(const BigInteger& rhs)
	{
		if (this != &rhs) {
			sign = rhs.sign;
			digits = rhs.digits;
		}
		return *this;
	}
	BigInteger& operator=(BigInteger&& rhs) noexcept
	{
		if (this != &rhs) {
			sign = rhs.sign;
			digits = std::move(rhs.digits);
		}
		return *this;
	}

	explicit operator bool() const
	{
		return !isZero();
	}

	BigInteger operator-() const
	{
		BigInteger result(*this);
		if (!isZero()) {
			result.sign = -result.sign;
		}
		return result;
	}
	static BigInteger& One()
	{
		static BigInteger one(1);
		return one;
	}
	BigInteger& operator++()
	{
		return operator+=(One());
	}
	BigInteger operator++(int)
	{
		BigInteger result(*this);
		operator++();
		return result;
	}
	BigInteger& operator--()
	{
		return operator-=(One());
	}
	BigInteger operator--(int)
	{
		BigInteger result(*this);
		operator--();
		return result;
	}

	BigInteger& operator+=(const BigInteger& rhs)
	{
		add(rhs);
		return *this;
	}
	BigInteger& operator-=(const BigInteger& rhs)
	{
		sub(rhs);
		return *this;
	}

	BigInteger& operator*=(const BigInteger& rhs)
	{
		return *this = *this * rhs;
	}
	BigInteger& operator/=(const BigInteger& rhs)
	{
		return *this = operator/(*this, rhs);
	}
	BigInteger& operator%=(const BigInteger& rhs)
	{
		return *this = operator%(*this, rhs);
	}

	std::string toString() const
	{
		if (isZero()) {
			return "0";
		}
		std::stringstream ss;
		if (sign < 0) ss << "-";
		for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
			ss << *it << std::setw(BASE_DIGITS) << std::setfill('0');
		}
		return ss.str();
	}

	void swap(BigInteger& rhs) noexcept
	{
		std::swap(sign, rhs.sign);
		std::swap(digits, rhs.digits);
	}

	static BigInteger gcd(BigInteger a, BigInteger b)
	{
		a.sign = 1;
		b.sign = 1;
		int cmp = compare(a, b);
		if (cmp == 0) {
			return a;
		} else if (cmp < 0) {
			a.swap(b);
		}
		while (!b.isZero()) {
			a %= b;
			a.swap(b);
		}
		return a;
	}

	bool isZero() const
	{
		return digits.size() == 0;
	}

	bool isOne() const
	{
		return sign == 1 && digits.size() == 1 && digits[0] == 1;
	}

	bool isNegative() const
	{
		return sign < 0;
	}

	bool isPositive() const
	{
		return sign > 0 && !isZero();
	}

	void negate()
	{
		if (!isZero()) {
			sign = -sign;
		}
	}

	static int countDecimalDigit(int v)
	{
		int count = 0;
		while (v != 0) {
			v /= 10;
			++count;
		}
		return count;
	}

	int countDecimalDigit() const
	{
		if (isZero()) return 1;
		return (int(digits.size()) - 1) * BASE_DIGITS + countDecimalDigit(digits.back());
	}

	static int powerOf10(int power)
	{
		assert(power >= 0 && power < 10);
		static int powers[] = { 1, 10, 100, 1000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000 };
		return powers[power];
	}

	BigInteger mulByPowerOf10(int power) const
	{
		if (isZero()) {
			return *this;
		}
		int factor = powerOf10(power % BASE_DIGITS);
		BigInteger res = *this * BigInteger(factor);
		res.digits.insert(res.digits.begin(), power / BASE_DIGITS, 0);
		return res;
	}

	static int compare(const BigInteger& lhs, const BigInteger& rhs)
	{
		int diff = lhs.sign - rhs.sign;
		if (diff != 0) {
			return diff;
		}
		return absCompare(lhs, rhs) * lhs.sign;
	}

	friend BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs);
	friend BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs);
	friend BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs);
	friend BigInteger operator&(const BigInteger& lhs, const BigInteger& rhs);
	friend BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs);
	friend BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs);

private:
	int size() const
	{
		return int(digits.size());
	}

	void setDigit(int pos, int digit)
	{
		assert(pos >= 0 && digit >= 0 && digit < BASE);
		if (size() < pos + 1) {
			digits.resize(pos + 1);
		}
		digits[pos] = digit;
	}

	int getDigit(int pos)
	{
		assert(pos >= 0);
		return pos < size() ? digits[pos] : 0;
	}

	static int absCompare(const BigInteger& lhs, const BigInteger& rhs, int rhsShift = 0)
	{
		assert(rhsShift >= 0);
		int diff = lhs.size() - rhs.size() - rhsShift;
		if (diff != 0) {
			return diff;
		}
		auto lit = lhs.digits.crbegin();
		auto rit = rhs.digits.crbegin();
		for (; rit != rhs.digits.crend(); void(++lit), void(++rit)) {
			diff = *lit - *rit;
			if (diff != 0) {
				return diff;
			}
		}
		for (; lit != lhs.digits.crend(); ++lit) {
			if (*lit != 0) {
				return *lit;
			}
		}
		return 0;
	}

	void normalize()
	{
		while (!digits.empty() && digits.back() == 0) {
			digits.pop_back();
		}
		if (isZero()) {
			sign = 1;
		}
	}

	void add(const BigInteger& rhs, int rhsShift = 0)
	{
		if (sign == rhs.sign) {
			addUnsigned(rhs, 1, 1, rhsShift);
		} else if (absCompare(*this, rhs, rhsShift) >= 0) {
			addUnsigned(rhs, 1, -1, rhsShift);
		} else {
			addUnsigned(rhs, -1, 1, rhsShift);
			sign = -sign;
		}
	}

	void sub(const BigInteger& rhs, int rhsShift = 0)
	{
		if (sign != rhs.sign) {
			addUnsigned(rhs, 1, 1, rhsShift);
		} else if (absCompare(*this, rhs) >= 0) {
			addUnsigned(rhs, 1, -1, rhsShift);
		} else {
			addUnsigned(rhs, -1, 1, rhsShift);
			sign = -sign;
		}
	}

	static int addWithCarry(int op1, int op2, int& carry)
	{
		int64_t result = int64_t(op1) + op2 + carry;
		int digit = (result % BASE + BASE) % BASE;
		carry = int((result - digit) / BASE);
		return digit;
	}

	static int mulWithCarry(int op1, int op2, int op3, int& carry)
	{
		int64_t result = int64_t(op1) * op2 + op3 + carry;
		int digit = (result % BASE + BASE) % BASE;
		carry = int((result - digit) / BASE);
		return digit;
	}

	void addUnsigned(const BigInteger& rhs, int lhsFactor, int rhsFactor, int rhsShift = 0)
	{
		assert(rhsShift >= 0);
		int carry = 0;
		int pos = 0;
		for (; pos < rhsShift; ++pos) {
			setDigit(pos, addWithCarry(lhsFactor * getDigit(pos), 0, carry));
		}
		for (; pos < int(rhs.size()) + rhsShift; ++pos) {
			setDigit(pos, addWithCarry(lhsFactor * getDigit(pos), rhsFactor * rhs.digits[pos - rhsShift], carry));
		}
		for (; pos < size() && carry != 0; ++pos) {
			setDigit(pos, addWithCarry(lhsFactor * getDigit(pos), 0, carry));
		}
		assert(carry >= 0);
		if (carry != 0) {
			setDigit(pos, carry);
		}
		normalize();
	}

public:
	static BigInteger multiply(const BigInteger& lhs, const BigInteger& rhs)
	{
		BigInteger res;
		if (lhs.isZero() || rhs.isZero()) return res;
		res.sign = lhs.sign * rhs.sign;
		for (int rhsPos = 0; rhsPos < rhs.size(); ++rhsPos) {
			int carry = 0;
			int lhsPos = 0;
			int resPos = rhsPos;
			for (; lhsPos < lhs.size(); void(++lhsPos), void(++resPos)) {
				res.setDigit(resPos, mulWithCarry(lhs.digits[lhsPos], rhs.digits[rhsPos], res.getDigit(resPos), carry));
			}
			assert(carry >= 0);
			if (carry != 0) {
				res.setDigit(resPos, carry);
			}
		}
		res.normalize();
		return res;
	}

	static BigInteger karatsubaAlgorithm(const BigInteger& lhs, const BigInteger& rhs)
	{
		int split = std::min(lhs.size(), rhs.size()) / 2;
		if (split == 0) {
			return multiply(lhs, rhs);
		}
		BigInteger low1(lhs, 0, split);
		BigInteger high1(lhs, split, lhs.size());
		BigInteger low2(rhs, 0, split);
		BigInteger high2(rhs, split, rhs.size());
		BigInteger z0 = karatsubaAlgorithm(low1, low2);
		BigInteger z2 = karatsubaAlgorithm(high1, high2);
		BigInteger z1 = karatsubaAlgorithm(low1 + high1, low2 + high2) - z0 - z2;
		z0.add(z1, split);
		z0.add(z2, split * 2);
		z0.normalize();
		return z0;
	}

	static void divide(const BigInteger& dividend, int divisor, BigInteger& quotient, int& remainder)
	{
		remainder = 0;
		if (divisor == 0) {
			quotient = BigInteger();
			return;
		} else if (divisor == 1) {
			quotient = dividend;
			return;
		} else if (divisor == -1) {
			quotient = -dividend;
			return;
		}
		assert(divisor > -BASE && divisor < BASE);
		int sign = 1;
		if (divisor < 0) {
			sign = -1;
			divisor = -divisor;
		}
		BigInteger lhs = dividend;
		quotient = BigInteger();
		for (int pos = lhs.size() - 1; pos >= 0; --pos) {
			int64_t d = int64_t(pos == lhs.size() - 1 ? 0 : lhs.digits[pos + 1]) * BASE + lhs.digits[pos];
			int64_t q = d / divisor;
			int64_t r = d - q * divisor;
			if (q != 0) {
				quotient.setDigit(pos, int(q));
			}
			lhs.digits[pos] = int(r);
		}
		remainder = lhs.getDigit(0) * lhs.sign;
		quotient.sign = sign * lhs.sign;
		quotient.normalize();
	}

	static void divide(const BigInteger& dividend, const BigInteger& divisor, BigInteger& quotient, BigInteger& remainder)
	{
		int dvs = divisor.size();
		if (dvs == 0) {
			quotient = BigInteger();
			remainder = BigInteger();
			return;
		}
		if (dvs == 1) {
			int r = 0;
			divide(dividend, divisor.digits[0] * divisor.sign, quotient, r);
			remainder = BigInteger(r);
			return;
		}
		int dd = countDecimalDigit(divisor.digits.back());
		int mb = powerOf10(BASE_DIGITS - dd);
		int db = powerOf10(dd);
		quotient = BigInteger();
		remainder = dividend;
		int64_t dv = int64_t(divisor.digits[dvs - 1]) * mb + (divisor.digits[dvs - 2] + db - 1) / db + (dvs > 2 ? 1 : 0);
		int64_t dv2 = int64_t(divisor.digits[dvs - 1]) * BASE + divisor.digits[dvs - 2] + (dvs > 2 ? 1 : 0);
		for (int rhsShift = remainder.size() - divisor.size(); rhsShift >= 0; --rhsShift) {
			int pos = divisor.size() + rhsShift;
			int64_t d = (int64_t(remainder.getDigit(pos)) * BASE + remainder.getDigit(pos - 1)) * mb + remainder.getDigit(pos - 2) / db;
			if (d >= dv) {
				auto q = BigInteger(d / dv);
				remainder.addUnsigned(divisor * q, 1, -1, rhsShift);
				quotient.addUnsigned(q, 1, 1, rhsShift);
			}
			d = int64_t(remainder.getDigit(pos - 1)) * BASE + remainder.getDigit(pos - 2);
			if (d >= dv2) {
				auto q = BigInteger(d / dv2);
				remainder.addUnsigned(divisor * q, 1, -1, rhsShift);
				quotient.addUnsigned(q, 1, 1, rhsShift);
			}
		}
		while (absCompare(remainder, divisor) >= 0) {
			remainder.addUnsigned(divisor, 1, -1, 0);
			quotient.addUnsigned(BigInteger(1), 1, 1, 0);
		}
		remainder.sign = dividend.sign;
		quotient.sign = divisor.sign * dividend.sign;
		remainder.normalize();
		quotient.normalize();
	}

	friend class Rational;
};

BigInteger operator""_bi(const char* s, size_t length)
{
	return BigInteger(s, length);
}

BigInteger operator""_bi(const char* s)
{
	return BigInteger(s, strlen(s));
}

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs)
{
	BigInteger result = lhs;
	result+=rhs;
	return result;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs)
{
	BigInteger result = lhs;
	result -= rhs;
	return result;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs)
{
	return BigInteger::karatsubaAlgorithm(lhs, rhs);
}
BigInteger operator&(const BigInteger& lhs, const BigInteger& rhs)
{
	return BigInteger::multiply(lhs, rhs);
}
BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs)
{
	BigInteger quotient, remainder;
	BigInteger::divide(lhs, rhs, quotient, remainder);
	return quotient;
}
BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs)
{
	BigInteger quotient, remainder;
	BigInteger::divide(lhs, rhs, quotient, remainder);
	return remainder;
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) == 0; }
bool operator!=(const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) != 0; }
bool operator< (const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) < 0; }
bool operator> (const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) > 0; }
bool operator<=(const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) <= 0; }
bool operator>=(const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) >= 0; }

std::istream& operator>>(std::istream& in, BigInteger& result)
{
	std::string s;
	in >> s;
	result = BigInteger(s.c_str(), s.size());
	return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& bi)
{
	return out << bi.toString();
}



class Rational
{
private:
	BigInteger num = BigInteger();
	BigInteger denom = BigInteger::One();

public:
	Rational()
	{}
	Rational(int64_t num_, int64_t denom_ = 1)
		: num(num_), denom(denom_)
	{
		normalize();
	}
	Rational(const BigInteger& num_, const BigInteger& denom_ = BigInteger::One())
		: num(num_), denom(denom_)
	{
		normalize();
	}
	Rational(BigInteger&& num_, BigInteger&& denom_)
		: num(std::move(num_)), denom(std::move(denom_))
	{
		normalize();
	}
	Rational(const Rational& rhs)
		: num(rhs.num), denom(rhs.denom)
	{}
	Rational(Rational&& rhs) noexcept
		: num(std::move(rhs.num)), denom(std::move(rhs.denom))
	{}

	Rational& operator=(const Rational& rhs)
	{
		if (this != &rhs) {
			num = rhs.num;
			denom = rhs.denom;
		}
		return *this;
	}
	Rational& operator=(Rational&& rhs) noexcept
	{
		if (this != &rhs) {
			num = std::move(rhs.num);
			denom = std::move(rhs.denom);
		}
		return *this;
	}

	explicit operator bool() const
	{
		return !isZero();
	}

	Rational operator-() const
	{
		return Rational(-num, denom);
	}

	Rational& operator+=(const Rational& rhs)
	{
		return *this = *this + rhs;
	}
	
	Rational& operator-=(const Rational& rhs)
	{
		return *this = *this - rhs;
	}
	Rational& operator*=(const Rational& rhs)
	{
		return *this = *this * rhs;
	}
	Rational& operator/=(const Rational& rhs)
	{
		return *this = *this / rhs;
	}

	std::string toString() const
	{
		if (isInteger()) {
			return num.toString();
		}
		return num.toString() + "/" + denom.toString();
	}

	std::string asDecimal(size_t precision = 0) const
	{
		std::stringstream ss;
		if (isInteger()) {
			ss << toString();
			if (precision != 0) {
				ss.put('.');
			}
			for (int i = 0; i < int(precision); ++i) {
				ss.put('0');
			}
			return ss.str();
		}
		BigInteger q, r;
		BigInteger d = num.mulByPowerOf10(int(precision));
		if (d.isNegative()) {
			d.negate();
			ss.put('-');
		}
		BigInteger::divide(d, denom, q, r);
#if 0
		// Округление
		if (r * BigInteger(2) >= denom) {
			++q;
		}
#endif
		auto s = q.toString();
		if (s.size() > precision) {
			ss.write(s.c_str(), s.size() - precision);
			if (precision != 0) {
				ss.put('.');
				ss.write(s.c_str() + s.size() - precision, precision);
			}
		} else {
			ss << "0.";
			for (int i = 0; i + s.size() < precision; ++i) {
				ss.put('0');
			}
			ss << s;
		}
		return ss.str();
	}

	explicit operator double() const
	{
		if (isZero()) {
			return 0.0;
		}
		int nn = num.countDecimalDigit();
		int nd = denom.countDecimalDigit();
		int precision = std::max(nd - nn + std::numeric_limits<double>::digits10 + 2, 0); // 2 для надёжности
		return stod(asDecimal(precision));
	}

	void swap(Rational& rhs) noexcept
	{
		std::swap(num, rhs.num);
		std::swap(denom, rhs.denom);
	}

	bool isZero() const
	{
		return num.isZero();
	}

	bool isInteger() const
	{
		return denom == BigInteger::One();
	}

	static int compare(const Rational& lhs, const Rational& rhs)
	{
		return BigInteger::compare(lhs.num * rhs.denom, rhs.num * lhs.denom);
	}

	friend Rational operator+(const Rational& lhs, const Rational& rhs);
	friend Rational operator-(const Rational& lhs, const Rational& rhs);
	friend Rational operator*(const Rational& lhs, const Rational& rhs);
	friend Rational operator/(const Rational& lhs, const Rational& rhs);

private:
	void normalize()
	{
		if (num.isZero() || denom.isZero()) { // 1/0 заменяется на 0/1
			num = BigInteger();
			denom = BigInteger::One();
			return;
		}
		if (denom.isNegative()) {
			num.negate();
			denom.negate();
		}
		if (!isInteger()) {
			auto g = BigInteger::gcd(num, denom);
			if (!g.isOne()) {
				num /= g;
				denom /= g;
			}
		}
	}
};


Rational operator+(const Rational& lhs, const Rational& rhs)
{
	return Rational(lhs.num * rhs.denom + lhs.denom * rhs.num, lhs.denom * rhs.denom);
}

Rational operator-(const Rational& lhs, const Rational& rhs)
{
	return Rational(lhs.num * rhs.denom - lhs.denom * rhs.num, lhs.denom * rhs.denom);
}

Rational operator*(const Rational& lhs, const Rational& rhs)
{
	return Rational(lhs.num * rhs.num, lhs.denom * rhs.denom);
}

Rational operator/(const Rational& lhs, const Rational& rhs)
{
	return Rational(lhs.num * rhs.denom, lhs.denom * rhs.num);
}

bool operator==(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) == 0; }
bool operator!=(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) != 0; }
bool operator< (const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) < 0; }
bool operator> (const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) > 0; }
bool operator<=(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) <= 0; }
bool operator>=(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) >= 0; }

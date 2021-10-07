#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


class BigInteger
{
private:
	static const int BASE_DIGIT = 2;
	static const int BASE = 1 << 20;
	static const int BASE_DIGITS = 20;
	static const int BASE_10 = 1'000'000;
	static const int BASE_10_DIGITS = 6;

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
		int digit = 0;
		for (int i = start; i < end; ++i) {
			if (s[i] == '\'') {
				continue;
			}
			digit = digit * 10 + (s[i] - '0');
			factor *= 10;
			if (factor >= BigInteger::BASE_10) {
				multiplyUnsignedSmall(factor, digit);
				factor = 1;
				digit = 0;
			}
		}
		multiplyUnsignedSmall(factor, digit);
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
		return *this = *this / rhs;
	}

	BigInteger& operator%=(const BigInteger& rhs)
	{
		return *this = *this % rhs;
	}

	std::string toString() const
	{
		if (isZero()) {
			return "0";
		}
		BigInteger copy(*this);
		std::string result;
		copy.sign = 1;
		int digit;
		while (!copy.isZero()) {
			divide(copy, BASE_10, copy, digit);
			for (int i = 0; i < BASE_10_DIGITS; ++i) {
				result.push_back(digit % 10 + '0');
				digit /= 10;
			}
		}
		while (result.size() > 1 && result.back() == '0') {
			result.pop_back();
		}
		if (sign < 0) {
			result.push_back('-');
		}
		reverse(result.begin(), result.end());
		return result;
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

	static BigInteger powerOfTwo(int power)
	{
		assert(BASE_DIGIT == 2);
		BigInteger result;
		while (power >= BASE_DIGITS) {
			result.digits.push_back(0);
			power -= BASE_DIGITS;
		}
		result.digits.push_back(1 << power);
		return result;
	}

	static BigInteger binaryGcd(BigInteger a, BigInteger b)
	{
		int commonBits = countCommonBits(a, b);
		a.logicalShiftRightBits(commonBits);
		b.logicalShiftRightBits(commonBits);
		BigInteger factor = powerOfTwo(commonBits);
		while (true) {
			int cmp = absCompare(a, b);
			if (a.isZero()) {
				std::swap(a, b);
				break;
			}
			if (b.isZero() || cmp == 0) {
				break;
			}
			if (a.isAbsOne() || b.isAbsOne()) {
				a = BigInteger::One();
				break;
			}
			if (a.digits[0] % BASE_DIGIT == 0) {
				while (a.digits[0] % BASE_DIGIT == 0) {
					a.logicalShiftRightBits(1);
				}
				continue;
			}
			if (b.digits[0] % BASE_DIGIT == 0) {
				while (b.digits[0] % BASE_DIGIT == 0) {
					b.logicalShiftRightBits(1);
				}
				continue;
			}
			if (cmp < 0) {
				std::swap(a, b);
			}
			a -= b;
			a.logicalShiftRightBits(1);
		}
		a.sign = 1;
		return a * factor;
	}

	static void reduceByGcd(BigInteger& a, BigInteger& b)
	{
		int commonDigits = countCommonDigits(a, b);
		a.logicalShiftRightDigits(commonDigits);
		b.logicalShiftRightDigits(commonDigits);
		auto g = binaryGcd(a, b);
		if (!g.isAbsOne()) {
			a /= g;
			b /= g;
		}
	}

	void logicalShiftRightBits(int count)
	{
		assert(count >= 0);
		if (count == 0 || isZero()) {
			return;
		}
		int shift = count / BASE_DIGITS;
		count %= BASE_DIGITS;
		if (size() <= shift) {
			*this = BigInteger();
			return;
		}
		assert(BASE_DIGIT == 2);
		for (int i = shift; i < size(); ++i) {
			digits[i - shift] = (digits[i] >> count) | ((getDigit(i + 1) << (BASE_DIGITS - count)) & (BASE - 1));
		}
		digits.resize(size() - shift);
		normalize();
	}

	void logicalShiftRightDigits(int count)
	{
		assert(count >= 0);
		if (count == 0 || isZero()) {
			return;
		}
		if (size() <= count) {
			*this = BigInteger();
			return;
		}
		digits.erase(digits.begin(), digits.begin() + count);
	}

	static int countCommonBits(const BigInteger& a, const BigInteger& b)
	{
		int i = 0;
		while (i < a.size() && i < b.size() && a.digits[i] == 0 && b.digits[i] == 0) {
			++i;
		}
		int count = i * BASE_DIGITS;
		int aDigit = a.getDigit(i);
		int bDigit = b.getDigit(i);
		while (aDigit % BASE_DIGIT == 0 && bDigit % BASE_DIGIT == 0) {
			aDigit /= BASE_DIGIT;
			bDigit /= BASE_DIGIT;
			++count;
		}
		return count;
	}

	static int countCommonDigits(const BigInteger& a, const BigInteger& b)
	{
		int i = 0;
		while (i < a.size() && i < b.size() && a.digits[i] == 0 && b.digits[i] == 0) {
			++i;
		}
		return i;
	}

	bool isZero() const
	{
		return digits.size() == 0;
	}

	bool isAbsOne() const
	{
		return digits.size() == 1 && digits[0] == 1;
	}

	bool isOne() const
	{
		return isAbsOne() && sign == 1;
	}

	bool isMinusOne() const
	{
		return isAbsOne() && sign == -1;
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
		if (size() < 2) {
			return countDecimalDigit(getDigit(0));
		}
		BigInteger copy(*this);
		int count = 0;
		while (copy.size() > 1) {
			int digit;
			divide(copy, BASE_10, copy, digit);
			count += BASE_10_DIGITS;
		}
		count += countDecimalDigit(copy.getDigit(0));
		return count;
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
		BigInteger res(*this);
		for (; power >= BASE_10_DIGITS; power -= BASE_10_DIGITS) {
			res.multiplyUnsignedSmall(BASE_10, 0);
		}
		res.multiplyUnsignedSmall(powerOf10(power), 0);
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

	int getDigit(int pos) const
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

	void multiplyUnsignedSmall(int rhs, int carry)
	{
		assert(rhs >= 0 && rhs < BASE);
		assert(carry >= 0 && carry < BASE);
		BigInteger res;
		if (rhs == 0) {
			*this = BigInteger();
		}
		if (rhs == 1 && carry == 0) {
			return;
		}
		for (int pos = 0; pos < size(); ++pos) {
			digits[pos] = mulWithCarry(digits[pos], rhs, 0, carry);
		}
		assert(carry >= 0);
		if (carry != 0) {
			digits.push_back(carry);
		}
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
			quotient = dividend;
			quotient.negate();
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
			int64_t d = int64_t(lhs.getDigit(pos + 1)) * BASE + lhs.digits[pos];
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
		quotient = BigInteger();
		remainder = dividend;
		uint64_t dv = uint64_t(divisor.digits[dvs - 1]) * BASE + divisor.digits[dvs - 2] + (dvs > 2 ? 1 : 0);
		for (int rhsShift = remainder.size() - divisor.size(); rhsShift >= 0; --rhsShift) {
			int pos = divisor.size() + rhsShift;
			uint64_t d = ((uint64_t(remainder.getDigit(pos + 1)) * BASE + remainder.getDigit(pos)) * BASE + remainder.getDigit(pos - 1)) * BASE + remainder.getDigit(pos - 2);
			if (d >= dv) {
				auto q = BigInteger(d / dv);
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

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs)
{
	BigInteger result = lhs;
	result += rhs;
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
	return BigInteger::multiply(lhs, rhs);
}

BigInteger operator&(const BigInteger& lhs, const BigInteger& rhs)
{
	return BigInteger::karatsubaAlgorithm(lhs, rhs);
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
bool operator< (const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) <  0; }
bool operator> (const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) >  0; }
bool operator<=(const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) <= 0; }
bool operator>=(const BigInteger& lhs, const BigInteger& rhs) { return BigInteger::compare(lhs, rhs) >= 0; }

BigInteger operator""_bi(const char* s, size_t length)
{
	return BigInteger(s, length);
}

BigInteger operator""_bi(const char* s)
{
	return BigInteger(s, strlen(s));
}

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

	void negate()
	{
		num.negate();
	}

	void invert()
	{
		if (!isZero()) {
			std::swap(num, denom);
			std::swap(num.sign, denom.sign);
		}
	}

	Rational operator-() const
	{
		Rational result(*this);
		result.negate();
		return result;
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

	bool isOne() const
	{
		return num.isOne() && denom.isOne();
	}

	bool isMinusOne() const
	{
		return num.isMinusOne() && denom.isOne();
	}

	bool isInteger() const
	{
		return denom.isOne();
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
		if (num.isZero() || denom.isZero()) { 
			num = BigInteger();
			denom = BigInteger::One();
			return;
		}
		if (denom.isNegative()) {
			num.negate();
			denom.negate();
		}
		if (!isInteger()) {
			BigInteger::reduceByGcd(num, denom);
		}
	}
};


Rational operator+(const Rational& lhs, const Rational& rhs)
{
	if (lhs.isZero()) {
		return rhs;
	} else if (rhs.isZero()) {
		return lhs;
	}
	return Rational(lhs.num * rhs.denom + lhs.denom * rhs.num, lhs.denom * rhs.denom);
}

Rational operator-(const Rational& lhs, const Rational& rhs)
{
	if (lhs.isZero()) {
		return -rhs;
	} else if (rhs.isZero()) {
		return lhs;
	}
	return Rational(lhs.num * rhs.denom - lhs.denom * rhs.num, lhs.denom * rhs.denom);
}

Rational operator*(const Rational& lhs, const Rational& rhs)
{
	if (lhs.isZero() || rhs.isZero()) {
		return Rational();
	} else if (lhs.isOne()) {
		return rhs;
	} else if (lhs.isMinusOne()) {
		return -rhs;
	} else if (rhs.isOne()) {
		return lhs;
	} else if (rhs.isMinusOne()) {
		return -lhs;
	}
	return Rational(lhs.num * rhs.num, lhs.denom * rhs.denom);
}

Rational operator/(const Rational& lhs, const Rational& rhs)
{
	if (lhs.isZero() || rhs.isZero()) {
		return Rational();
	} else if (lhs.isOne()) {
		Rational result(rhs);
		result.invert();
		return result;
	} else if (lhs.isMinusOne()) {
		Rational result(rhs);
		result.invert();
		result.negate();
		return result;
	} else if (rhs.isOne()) {
		return lhs;
	} else if (rhs.isMinusOne()) {
		return -lhs;
	}
	return Rational(lhs.num * rhs.denom, lhs.denom * rhs.num);
}

bool operator==(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) == 0; }
bool operator!=(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) != 0; }
bool operator< (const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) <  0; }
bool operator> (const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) >  0; }
bool operator<=(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) <= 0; }
bool operator>=(const Rational& lhs, const Rational& rhs) { return Rational::compare(lhs, rhs) >= 0; }

std::istream& operator>>(std::istream& in, Rational& result)
{
	std::string s;
	in >> s;
	result = Rational(BigInteger(s.c_str(), s.size()));
	return in;
}

std::ostream& operator<<(std::ostream& out, const Rational& bi)
{
	return out << bi.toString();
}



template<int N>
class Finite
{
private:
    int x = 0;

	static int pow(int a, int b)
	{
		int64_t r = 1;
		int64_t f = a;
		while (b > 0) {
			if (b & 1) {
				r = r * f % N;
			}
			f = f * f % N;
			b /= 2;
		}
		return int(r);
	}

	static int rev(int a)
	{
		return pow(a, N - 2);
	}

public:
	Finite(int64_t x_)
		: x((x_% N + N) % N)
	{}

	Finite(const Finite& rhs)
	{
		x = rhs.x;
	}

	Finite& operator=(const Finite& rhs)
	{
		if (this != &rhs) {
			x = rhs.x;
		}
		return *this;
	}

	int get() const
	{
		return x;
	}

	Finite operator-() const
	{
		return Finite(int64_t(N) - x);
	}

	Finite& operator+=(const Finite& rhs)
	{
		x = (int64_t(x) + rhs.x) % N;
		return *this;
	}

	Finite& operator-=(const Finite& rhs)
	{
		x = (int64_t(x) + N - rhs.x) % N;
		return *this;
	}

	Finite& operator*=(const Finite& rhs)
	{
		x = int64_t(x) * rhs.x % N;
		return *this;
	}

	Finite& operator/=(const Finite& rhs)
	{
		x = int64_t(x) * rev(rhs.x) % N;
		return *this;
	}

	Finite& operator++()
	{
		return *this += 1;
	}

	Finite operator++(int)
	{
		Finite result(*this);
		operator++();
		return result;
	}

	Finite& operator--()
	{
		return *this -= 1;
	}

	Finite operator--(int)
	{
		Finite result(*this);
		operator--();
		return result;
	}
};

template<int N>
bool operator==(const Finite<N>& lhs, const Finite<N>& rhs)
{
	return lhs.get() == rhs.get();
}

template<int N>
bool operator!=(const Finite<N>& lhs, const Finite<N>& rhs)
{
	return lhs.get() != rhs.get();
}

template<int N>
Finite<N> operator+(const Finite<N>& lhs, const Finite<N>& rhs)
{
	Finite<N> result = lhs;
	result.operator+=(rhs);
	return result;
}

template<int N>
Finite<N> operator-(const Finite<N>& lhs, const Finite<N>& rhs)
{
	Finite<N> result = lhs;
	result.operator-=(rhs);
	return result;
}

template<int N>
Finite<N> operator*(const Finite<N>& lhs, const Finite<N>& rhs)
{
	Finite<N> result = lhs;
	result.operator*=(rhs);
	return result;
}

template<int N>
Finite<N> operator/(const Finite<N>& lhs, const Finite<N>& rhs)
{
	Finite<N> result = lhs;
	result.operator/=(rhs);
	return result;
}

template<int N>
std::ostream& operator<<(std::ostream& os, const Finite<N>& rhs)
{
	os << rhs.get();
	return os;
}



template<unsigned M, unsigned N, typename Field = Rational>
class Matrix;
template<unsigned M, unsigned N, typename Field = Rational>
std::ostream& operator<<(const std::ostream& os, const Matrix<M, N, Field>& rhs);


template<unsigned M, unsigned N, typename Field>
class Matrix
{
private:
	std::vector<std::vector<Field>> data;

	void normalize()
	{
		data.resize(M, std::vector<Field>(N, Field(0)));
		for (int i = 0; i < int(M); ++i) {
			data[i].resize(N, Field(0));
		}
	}

	void addToRow(int i, int i1, Field factor)
	{
		for (int j = 0; j < int(N); ++j) {
			data[i][j] += data[i1][j] * factor;
		}
	}

	void multRow(int i, Field factor)
	{
		for (int j = 0; j < int(N); ++j) {
			data[i][j] *= factor;
		}
	}

	void subtractRows(int i, Field f, int k, Field g)
	{
		for (int j = 0; j < int(N); ++j) {
			data[i][j] = data[i][j] * f - data[k][j] * g;
		}
	}

public:
	Matrix()
		: data(M, std::vector<Field>(N, Field(0)))
	{
		for (int i = 0; i < int(M) && i < int(N); ++i) {
			data[i][i] = Field(1);
		}
	}

	Matrix(Field x)
		: data(M, std::vector<Field>(N, x))
	{ }

	Matrix(const Matrix& rhs)
		: data(rhs.data)
	{ }

	Matrix(Matrix&& rhs) noexcept
		: data(move(rhs.data))
	{ }

	Matrix(const std::vector<std::vector<Field>>& rhs)
		: data(rhs)
	{
		normalize();
	}

	Matrix(std::vector<std::vector<Field>>&& rhs) noexcept
		: data(move(rhs))
	{
		normalize();
	}

	Matrix(const std::vector<std::vector<int>>& rhs)
		: Matrix(Field(0))
	{
		normalize();
		for (int i = 0; i < int(M) && i < int(rhs.size()); ++i) {
			for (int j = 0; j < int(N) && j < int(rhs[i].size()); ++j) {
				data[i][j] = Field(rhs[i][j]);
			}
		}
	}

	Matrix& operator=(const Matrix& rhs)
	{
		if (this != &rhs) {
			data = rhs.data;
		}
		return *this;
	}

	Matrix& operator=(Matrix&& rhs) noexcept
	{
		if (this != &rhs) {
			data = move(rhs.data);
		}
		return *this;
	}

	std::vector<Field> getRow(unsigned i)
	{
		return std::vector<Field>(data[i]);
	}

	std::vector<Field> getColumn(unsigned j)
	{
		std::vector<Field> result(M);
		for (int i = 0; i < int(M); ++i) {
			result[i] = data[i][j];
		}
		return result;
	}

	const std::vector<Field>& operator[](size_t i) const
	{
		return data[i];
	}

	std::vector<Field>& operator[](size_t i)
	{
		return data[i];
	}

	bool operator==(const Matrix& rhs) const
	{
		return data == rhs.data;
	}

	bool operator!=(const Matrix& rhs) const
	{
		return data != rhs.data;
	}

	Matrix operator-() const
	{
		return *this * Field(-1);
	}

	Matrix& operator+=(const Matrix& rhs)
	{
		Matrix result(Field(0));
		for (int i = 0; i < int(M); ++i) {
			for (int j = 0; j < int(N); ++j) {
				data[i][j] += rhs.data[i][j];
			}
		}
		return *this;
	}

	Matrix operator+(const Matrix& rhs) const
	{
		Matrix result = *this;
		result.operator+=(rhs);
		return result;
	}

	Matrix& operator-=(const Matrix& rhs)
	{
		Matrix result(Field(0));
		for (int i = 0; i < int(M); ++i) {
			for (int j = 0; j < int(N); ++j) {
				data[i][j] -= rhs.data[i][j];
			}
		}
		return *this;
	}

	Matrix operator-(const Matrix& rhs) const
	{
		Matrix result = *this;
		result.operator-=(rhs);
		return result;
	}

	Matrix& operator*=(const Field& rhs)
	{
		for (int i = 0; i < int(M); ++i) {
			for (int j = 0; j < int(N); ++j) {
				data[i][j] *= rhs;
			}
		}
		return *this;
	}

	Matrix operator/(const Field& rhs) const
	{
		Matrix result = *this;
		result.operator/=(rhs);
		return result;
	}

	Matrix& operator/=(const Field& rhs)
	{
		for (int i = 0; i < int(M); ++i) {
			for (int j = 0; j < int(N); ++j) {
				data[i][j] /= rhs;
			}
		}
		return *this;
	}

	Matrix operator*(const Field& rhs) const
	{
		Matrix result = *this;
		result.operator*=(rhs);
		return result;
	}

	Matrix& operator*=(const Matrix<N, N, Field>& rhs)
	{
		Matrix result = *this * rhs;
		*this = result;
		return *this;
	}

	template<unsigned K>
	Matrix<M, K, Field> operator*(const Matrix<N, K, Field>& rhs) const
	{
		Matrix<M, K, Field> result(Field(0));
		for (int i = 0; i < int(M); ++i) {
			for (int k = 0; k < int(K); ++k) {
				for (int j = 0; j < int(N); ++j) {
					result[i][k] += data[i][j] * rhs[j][k];
				}
			}
		}
		return result;
	}

	template<bool Valid = (M == N)>
	typename std::enable_if_t<Valid, Field> det() const
	{
		return Matrix(*this).gaussStep1(true).second;
	}

	Matrix<N, M, Field> transposed() const
	{
		Matrix<N, M, Field> result(Field(0));
		for (int i = 0; i < int(M); ++i) {
			for (int j = 0; j < int(N); ++j) {
				result[j][i] = data[i][j];
			}
		}
		return result;
	}

	int rank() const
	{
		return Matrix(*this).gaussStep1().first;
	}

	Field trace() const
	{
		Field result(0);
		for (int i = 0; i < int(M) && i < int(N); ++i) {
			result += data[i][i];
		}
		return result;
	}

	template<bool Valid = (M == N)>
	typename std::enable_if_t<Valid, Matrix> inverted() const
	{
		Matrix copy = *this;
		copy.invert();
		return copy;
	}

	template<bool Valid = (M == N)>
	typename std::enable_if_t<Valid> invert()
	{
		Matrix<M, M * 2, Field> ext(data);
		for (int i = 0; i < int(M); ++i) {
			ext[i][M + i] = Field(1);
		}
		Field det = ext.gaussStep1().second;
		if (det == Field(0)) {
			return;
		}
		ext.gaussStep2();
		for (int i = 0; i < int(M); ++i) {
			for (int j = 0; j < int(M); ++j) {
				data[i][j] = ext[i][M + j];
			}
		}
	}

	std::pair<int, Field> gaussStep1(bool detOnly = false)
	{
		int rank = 0;
		Field det(1);
		for (int j = 0; j < int(N); ++j) {
			for (int i = rank; i < int(M); ++i) {
				if (data[i][j] != Field(0)) {
					if (i != rank) {
						swap(data[i], data[rank]);
						det *= -1;
					}
					if (rank == j) {
						det *= data[rank][j];
					}
					for (int i1 = rank + 1; i1 < int(M); ++i1) {
						addToRow(i1, rank, -data[i1][j] / data[rank][j]);
					}
					++rank;
					break;
				}
				if (i + 1 == int(M)) {
					det = Field(0);
					if (detOnly) {
						return { 0, Field(0) };
					}
				}
			}
		}
		return { rank, det };
	}

	void gaussStep2()
	{
		for (int i = M - 1; i >= 0 && data[i][i] != Field(0); --i) {
			multRow(i, Field(1) / data[i][i]);
			for (int i1 = i - 1; i1 >= 0; --i1) {
				addToRow(i1, i, -data[i1][i]);
			}
		}
	}

	friend std::ostream& operator<< <M, N, Field>(const std::ostream& os, const Matrix<M, N, Field>& rhs);

};

template<unsigned M, unsigned N, typename Field = Rational>
Matrix<M, N, Field> operator*(const Field& lhs, const Matrix<M, N, Field>& rhs)
{
	return rhs * lhs;
}

template<unsigned M, unsigned N, typename Field = Rational>
std::ostream& operator<<(std::ostream& os, const Matrix<M, N, Field>& rhs)
{
	os << "{";
	for (int i = 0; i < int(M); ++i) {
		os << "{";
		for (int j = 0; j < int(N); ++j) {
			os << rhs[i][j];
			os << (j + 1 == int(N) ? "}" : ",");
		}
		os << (i + 1 == int(M) ? "}" : ",");
	}
	return os;
}



template<unsigned N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;

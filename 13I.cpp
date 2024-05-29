#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

int gcd(int a, int b)
{
	return !b ? a : gcd(b, a % b);
}


template<int MOD>
class GF
{
	int v;

public:
	GF()
		: v(0)
	{
	}

	GF(const GF& other)
		: v(other.v)
	{
	}

	explicit GF(int64_t a)
		: v(a >= 0 ? a % MOD : (a % MOD + MOD) % MOD)
	{
	}

	GF& operator=(const GF& other)
	{
		if (this != &other) {
			v = other.v;
		}
		return *this;
	}

	GF& operator=(int64_t a)
	{
		*this = GF(a);
		return *this;
	}

	bool operator==(const GF& other) const
	{
		return v == other.v;
	}

	bool operator!=(const GF& other) const
	{
		return v != other.v;
	}

	bool operator<(const GF& other) const
	{
		return v < other.v;
	}

	GF& operator+=(const GF& other)
	{
		v = (v + other.v) % MOD;
		return *this;
	}

	GF operator+(const GF& other) const
	{
		return GF(v + other.v);
	}

	GF& operator-=(const GF& other)
	{
		v = (v + MOD - other.v) % MOD;
		return *this;
	}

	GF operator-(const GF& other) const
	{
		return GF(v - other.v);
	}

	GF& operator*=(const GF& other)
	{
		v = (1LL * v * other.v) % MOD;
		return *this;
	}

	GF operator*(const GF& other) const
	{
		return GF(1LL * v * other.v);
	}

	GF power(int64_t k) const
	{
		k %= MOD - 1;
		GF r(1);
		GF m(*this);
		while (k != 0) {
			if (k & 1) {
				r *= m;
			}
			m *= m;
			k /= 2;
		}
		return r;
	}

	GF operator~() const
	{
		return this->power(MOD - 2);
	}

	GF operator-() const
	{
		return GF(MOD - v);
	}

	int operator*() const
	{
		return v;
	}

	int signed_value() const
	{
		return v <= MOD / 2 ? v : v - MOD;
	}

	static constexpr int module = MOD;
};


constexpr int BIG_MOD = 7'340'033;

GF<BIG_MOD> operator""_r(unsigned long long v)
{
	return GF<BIG_MOD>(v % BIG_MOD);
}

vector<GF<BIG_MOD>> ROOTS = {
	7340032_r, 2306278_r, 2001861_r, 1627581_r, 3353052_r,
	3114847_r, 2515483_r, 1357781_r,  680314_r, 3513903_r,
	1736463_r,  153766_r, 2743563_r,  907050_r, 1504761_r,
	2903335_r, 1434658_r,  338192_r,  746933_r, 3478878_r,
}; // корни степеней 2^{k+1} для 1

vector<GF<BIG_MOD>> REVERSED_ROOTS = {
	7340032_r, 5033755_r, 3413510_r, 4961214_r, 7295050_r,
	6067369_r, 5416304_r, 6777965_r, 3629816_r, 1947550_r,
	1711230_r, 3489196_r, 2046946_r, 6341884_r, 585335_r,
	431801_r, 2653924_r, 3970201_r, 5391104_r, 5020900_r,
}; // корни степеней -2^{k+1} для 1

int reverse_bits(int n, int bits)
{
	int v = 0;
	for (int i = 0; i < bits; ++i) {
		v = (v << 1) | (n & 1);
		n >>= 1;
	}
	return v;
}

void ntt(vector<GF<BIG_MOD>>& a, const vector<GF<BIG_MOD>>& roots = ROOTS)
{
	int N = int(a.size());
	int bits = 0;
	while ((1 << bits) < N) {
		++bits;
	}
	for (int i = 0; i < N; ++i) {
		int j = reverse_bits(i, bits);
		if (i < j) {
			swap(a[i], a[j]);
		}
	}
	bits = 0;
	for (int bs = 2; bs <= N; bs *= 2) // размер блока
	{
		GF<BIG_MOD> e = roots[bits]; // 1^(1/bs)
		for (int off = 0; off < N; off += bs) { // сдвиг блока
			GF<BIG_MOD> f(1); // e^j == 1^(j/bs)
			for (int j = 0; j < bs / 2; ++j) {
				auto x = a[off + j];
				auto y = a[off + j + bs / 2] * f;
				a[off + j] = x + y;
				a[off + j + bs / 2] = x + y * roots[0];
				f *= e;
			}
		}
		++bits;
	}
}

void intt(vector<GF<BIG_MOD>>& A, const vector<GF<BIG_MOD>>& roots = REVERSED_ROOTS)
{
	ntt(A, roots);
	auto m = ~GF<BIG_MOD>(A.size());
	transform(A.begin(), A.end(), A.begin(), [&](const GF<BIG_MOD>& x) { return x * m; });
}

template<class T>
class Poly
{
	vector<T> A;

public:
	Poly()
	{
	}

	Poly(const Poly& other)
		: A(other.A)
	{
	}

	Poly(Poly&& other)
		: A(move(other.A))
	{
	}

	Poly& operator=(const Poly& other)
	{
		if (this != &other) {
			A = other.A;
		}
		return *this;
	}

	Poly& operator=(Poly&& other)
	{
		A = move(other.A);
		return *this;
	}

	explicit Poly(const vector<T>& A)
		: A(A)
	{
		normalize();
	}

	explicit Poly(const vector<int>& V)
		: A(V.size())
	{
		transform(V.begin(), V.end(), A.begin(), [](int v) { return T(v); });
		normalize();
	}

	int size() const
	{
		return int(A.size());
	}

	int order() const
	{
		return size() - 1;
	}

	bool operator==(const Poly& other)
	{
		return A == other.A;
	}

	bool operator!=(const Poly& other)
	{
		return A != other.A;
	}

	Poly& normalize()
	{
		while (!A.empty() && *A.back() == 0) {
			A.pop_back();
		}
		return *this;
	}

	Poly& operator*=(const T& n)
	{
		transform(A.begin(), A.end(), A.begin(), [&](const T& a) { return a * n; });
		normalize();
		return *this;
	}

	Poly& operator+=(const Poly& other)
	{
		auto& B = other.A;
		if (A.size() < B.size()) {
			A.resize(B.size());
		}
		transform(B.begin(), B.end(), A.begin(), A.begin(), [](const T& b, const T& a) { return a + b; });
		normalize();
		return *this;
	}

	Poly& operator-=(const Poly& other)
	{
		auto& B = other.A;
		if (A.size() < B.size()) {
			A.resize(B.size());
		}
		transform(B.begin(), B.end(), A.begin(), A.begin(), [](const T& b, const T& a) { return a - b; });
		normalize();
		return *this;
	}

	void output(const char* comment = nullptr)
	{
		if (comment) {
			cout << comment << ": ";
		}
		if (A.empty()) {
			cout << "0 0\n";
		} else {
			cout << order();
			for (auto it = A.rbegin(); it != A.rend(); ++it) {
				cout << " " << **it;
			}
			cout << "\n";
		}
	}

	Poly operator^ (const Poly& other) const
	{
		Poly result;
		auto& B = other.A;
		auto& C = result.A;
		C.resize(A.size() + B.size() - 1);
		for (int i = 0; i < int(A.size()); ++i) {
			for (int j = 0; j < int(B.size()); ++j) {
				C[i + j] += A[i] * B[j];
			}
		}
		C.normalize();
		return result;
	}

	template<int OTHER_MOD>
	static vector<GF<OTHER_MOD>> module_transform(const vector<T>& A)
	{
		vector<GF<OTHER_MOD>> B(A.size());
		transform(A.begin(), A.end(), B.begin(), [](const T& a) {
			return GF<OTHER_MOD>(a.signed_value());
		});
		return B;
	}

	Poly operator*(const Poly& other) const
	{
		auto B = module_transform<BIG_MOD>(A);
		auto C = module_transform<BIG_MOD>(other.A);
		int sz = 1;
		while (sz < int(B.size() + C.size() - 1)) {
			sz *= 2;
		}
		B.resize(sz);
		C.resize(sz);
		ntt(B);
		ntt(C);
		transform(B.begin(), B.end(), C.begin(), B.begin(), [](const GF<BIG_MOD>& b, const GF<BIG_MOD>& c) { return b * c; });
		intt(B);
		auto R = Poly(Poly<GF<BIG_MOD>>::module_transform<T::module>(B));
		R.normalize();
		return R;
	}

	Poly& operator<<= (int k)
	{
		A.insert(A.begin(), k, T());
		return *this;
	}

	Poly& operator>>= (int k)
	{
		if (A.size() <= k) {
			A.resize(1);
			A[0] = T();
		} else {
			A.erase(A.begin(), A.begin() + k);
		}
		return *this;
	}

private:
	Poly reciprocal() const
	{
		int sz = int(A.size()); // sz - степень двойки
		if (sz == 1) {
			return Poly({ ~A[0] });
		}
		auto Q = Poly(vector<T>(A.begin() + sz / 2, A.end())).reciprocal(); // деление на старшую половину многочлена P
		auto R = *this * Q * Q;
		Q <<= sz * 3 / 2 - 2; // * x^(3 / 2 * sz - 2)
		Q *= T(2);
		Q -= R;
		Q >>= sz - 2; // / x^(sz - 2)
		return Q.normalize();
	}

public:
	Poly operator/(const Poly& divisor)
	{
		int NA = order();
		int NB = divisor.order();
		if (NA < NB || NB < 0) {
			return Poly();
		}


		int sz1 = 1;
		while (sz1 < NB + 1 || sz1 < NA - NB + 1) {
			sz1 *= 2;
		}
		auto div = divisor;
		div <<= (sz1 - (NB + 1));

		auto S = div.reciprocal();
		S >>= sz1 - (NA - NB + 1);

		auto Q = *this * S;
		Q >>= NA;
		return Q;
	}

	Poly reminder(const Poly& divider, const Poly& quotient)
	{
		auto result = *this;
		result -= divider * quotient;
		return result;
	}
};

void stress()
{
	for (;;) {
		vector<GF<7>> A(50000); // A(rand() % 10 + 2);
		for (auto& v : A) {
			v = GF<7>(rand());
		}
		A.emplace_back(rand() % 7 + 1);

		vector<GF<7>> B(25000); // rand() % 10 + 2);
		for (auto& v : B) {
			v = GF<7>(rand());
		}
		B.emplace_back(rand() % 7 + 1);

		Poly<GF<7>> PA(A);
		Poly<GF<7>> PB(B);

		auto PQ = PA / PB;
		auto PR = PA;
		PR -= PB * PQ;

		auto PC = PQ * PB;
		PC += PR;

		if (PR.order() > PB.order()) {
			cout << "test failed: PC.order() > PB.order()\n";
			PA.output("PA");
			PB.output("PB");
			PQ.output("PQ");
			PR.output("PR");
			PC.output("PC");
			exit(1);
		}

		if (PA != PC) {
			cout << "test failed: PA != PC\n";
			PA.output("PA");
			PB.output("PB");
			PQ.output("PQ");
			PR.output("PR");
			PC.output("PC");
			exit(1);
		}

		if (PA.size() < 15 && PB.size() < 15) {
			PA.output("PA");
			PB.output("PB");
		} else {
			cout << "PA.size:" << PA.size() << " PB.size:" << PB.size() << "\n";
		}
	}
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int KA;
	cin >> KA;
	vector<int> A(KA + 1);
	for (auto& a : A) {
		cin >> a;
	}
	reverse(A.begin(), A.end());
	auto dividend = Poly<GF<7>>(A);

	int KB;
	cin >> KB;
	vector<int> B(KB + 1);
	for (auto& b : B) {
		cin >> b;
	}
	reverse(B.begin(), B.end());
	auto divisor = Poly<GF<7>>(B);

	auto quotient = dividend / divisor;
	quotient.output();

	auto reminder = dividend;
	reminder -= quotient * divisor;
	reminder.output();

	return 0;
}
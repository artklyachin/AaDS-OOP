#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <numeric>
#include <cmath>
#include <cassert>
#include <tuple>
#include <iterator>
#include <array>

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
		: v(a% MOD)
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
		return GF(v - other.v + MOD);
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

template<int MOD>
GF<MOD> min_primitive_root(int PHI)
{
	using T = GF<MOD>;
	vector<int> factors; // делители phi
	int n = PHI;
	for (int i = 2; i * i <= n; ++i) {
		if (n % i == 0) {
			factors.push_back(i);
			while (n % i == 0) {
				n /= i;
			}
		}
	}
	if (n != 1) {
		factors.push_back(n);
	}

	for (int g = 2; g < MOD; ++g) {
		bool flag = true;
		for (size_t i = 0; i < factors.size() && flag; ++i) {
			if (T(g).power(PHI / factors[i]) == T(1)) {
				flag = false;
				break;
			}
		}
		if (flag) {
#ifdef _DEBUG
			cerr << "primitive_root = " << g << "\n";
#endif
			return T(g);
		}
	}
#ifdef _DEBUG
	cerr << "no primitive root was found\n";
#endif
	return T();
}

template<int MOD>
vector<int> discrete_log(const GF<MOD>& a, const GF<MOD>& b)
{
	using T = GF<MOD>;
	vector<int> ans;
	int sq = int(sqrt(double(MOD))) + 1;
	auto asq = a.power(sq);
	map<T, int> values;
	auto cur = asq;
	for (int i = 1; i <= sq; ++i) {
		values.emplace(cur, i * sq);
		cur *= asq;
	}
	cur = b;
	for (int i = 0; i <= sq; ++i) {
		auto it = values.find(cur);
		if (it != values.end()) {
			int x = it->second - i;
			if (x < MOD - 1) {
				ans.emplace_back(x);
			}
			assert(a.power(x) == b);
		}
		cur *= a;
	}
	sort(ans.begin(), ans.end());
#ifdef _DEBUG
	cerr << "discrete_log(" << *a << "^x=" << *b << ") = {";
	for (auto v : ans) {
		cerr << v << ", ";
	}
	cerr << "}\n";
#endif
	return ans;
}

template<int MOD>
vector<GF<MOD>> discrete_root(int k, const GF<MOD>& a, int PHI, const GF<MOD>& G)
{
	using T = GF<MOD>;
	if (a == T(0)) {
		return { a };
	}
	auto log_ans = discrete_log(G.power(k), a);
	if (log_ans.empty()) {
		return {};
	}
	int y = log_ans[0];
	int delta = PHI / gcd(PHI, k);
	vector<T> ans;
	for (int cur = y % delta; cur < MOD - 1; cur += delta) {
		auto x = G.power(cur);
		ans.emplace_back(x);
		assert(x.power(k) == a);
	}
	sort(ans.begin(), ans.end());
#ifdef _DEBUG
	cerr << "discrete root(x^" << k << "=" << *a << ") = {";
	for (auto v : ans) {
		cerr << *v << ", ";
	}
	cerr << "}\n";
#endif
	return ans;
}

template<int MOD>
void precalculate()
{
	using T = GF<MOD>;
	int PHI = MOD - 1; // ожидаем MOD простой
	auto G = min_primitive_root<MOD>(PHI);
	T cur(1);
	vector<T> roots;
	while (true) {
		auto v = discrete_root(2, cur, PHI, G);
		if (v.empty()) {
			break;
		}
		auto root = v[0] != cur ? v[0] : v[1];
		roots.emplace_back(root);
		cur = root;
	}
#ifdef _DEBUG
	cerr << "roots = {";
	for (auto v : roots) {
		cerr << *v << "_r, ";
	}
	cerr << "}\n";
	cerr << "rev_roots = {";
	for (auto v : roots) {
		cerr << *~v << "_r, ";
	}
	cerr << "}\n";
#endif
}


constexpr int BIG_MOD = 998'244'353;

GF<BIG_MOD> operator""_r(unsigned long long v)
{
	return GF<BIG_MOD>(v % BIG_MOD);
}

vector<GF<BIG_MOD>> ROOTS = {
	998244352_r,  86583718_r, 488723995_r, 369330050_r, 454590761_r,
	399820479_r, 249745217_r, 257528000_r,  95199312_r, 119470575_r,
	176492632_r, 195232990_r, 381848112_r, 433524747_r, 380991880_r,
	237243495_r, 415196187_r, 313960847_r,  88894187_r,  53135229_r,
	137539336_r, 469954555_r,  98780269_r,
}; // корни степеней 2^{k+1} для 1

vector<GF<BIG_MOD>> REVERSED_ROOTS = {
	998244352_r, 911660635_r, 625715529_r, 373294451_r, 170256584_r,
	677566219_r, 588636374_r, 330946885_r, 307851042_r, 752918925_r,
	589438697_r, 697198720_r, 337850744_r, 312724106_r, 120823110_r,
	138766220_r, 289926285_r, 715445772_r, 959794845_r, 450486469_r,
	339515844_r, 846844265_r, 102392660_r,
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

vector<vector<int>> ReverseBits;
vector<GF<BIG_MOD>> RevPowers;

void ntt_prepare(int maxBits)
{
	ReverseBits.resize(maxBits + 1);
	RevPowers.resize(maxBits + 1);
	for (int i = 0; i <= maxBits; ++i) {
		int limit = 1 << i;
		ReverseBits[i].resize(limit);
		for (int j = 0; j < limit; ++j) {
			ReverseBits[i][j] = reverse_bits(j, i);
		}
		RevPowers[i] = ~GF<BIG_MOD>(limit);
	}
}

void ntt(vector<GF<BIG_MOD>>& a, int bits, const vector<GF<BIG_MOD>>& roots = ROOTS)
{
	int N = int(a.size());
	auto& RB = ReverseBits[bits];
	for (int i = 0; i < N; ++i) {
		int j = RB[i];
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
				a[off + j + bs / 2] = x - y;
				f *= e;
			}
		}
		++bits;
	}
}

void intt(vector<GF<BIG_MOD>>& A, int bits, const vector<GF<BIG_MOD>>& roots = REVERSED_ROOTS)
{
	ntt(A, bits, roots);
	auto m = RevPowers[bits];
	transform(A.begin(), A.end(), A.begin(), [&](const GF<BIG_MOD>& x) { return x * m; });
}

int signed_value(const int& v)
{
	return v;
}

int unsigned_value(const int& v)
{
	return v;
}

template<int MOD>
int signed_value(const GF<MOD>& v)
{
	return v.signed_value();
}

template<int MOD>
int unsigned_value(const GF<MOD>& v)
{
	return *v;
}

template<class T>
vector<T> convolution(vector<T> A, vector<T> B)
{
	int limit = int(A.size() + B.size() - 1);
	int bits = 0;
	while ((1 << bits) < limit) {
		++bits;
	}
	A.resize(1 << bits);
	B.resize(1 << bits);
	ntt(A, bits);
	ntt(B, bits);
	transform(A.begin(), A.end(), B.begin(), A.begin(), [](const T& a, const T& b) { return a * b; });
	intt(A, bits);
	A.resize(limit);
	return A;
}

template<>
vector<int> convolution(vector<int> A, vector<int> B)
{
	vector<int> C(A.size() + B.size() - 1);
	for (int i = 0; i < int(A.size()); ++i) {
		for (int j = 0; j < int(B.size()); ++j) {
			C[i + j] += A[i] * B[j];
		}
	}
	return C;
}

template<typename Iterator, typename ConstIterator>
vector<int> convolution_IT(
	Iterator A_begin,
	Iterator A_end,
	vector<GF<BIG_MOD>>& A,
	ConstIterator B_begin,
	ConstIterator B_end,
	vector<GF<BIG_MOD>>& B,
	int bits,
	int limit)
{
	using T = GF<BIG_MOD>;
	int sz = 1 << bits;
	if (int(A.size()) != sz) {
		A.assign(sz, GF<BIG_MOD>());
		transform(A_begin, A_end, A.begin(), [](const int& x) { return T(x); });
		ntt(A, bits);
	}
	if (int(B.size()) != sz) {
		B.assign(sz, GF<BIG_MOD>());
		transform(B_begin, B_end, B.begin(), [](const int& x) { return T(x); });
		ntt(B, bits);
	}
	vector<T> C(sz);
	transform(A.begin(), A.end(), B.begin(), C.begin(), [](const T& a, const T& b) { return a * b; });
	intt(C, bits);
	vector<int> result(limit);
	transform(C.begin(), C.begin() + limit, result.begin(), [](const T& x) { return *x; });
	return result;
}

struct Toeplitz
{
	vector<int> F;
	map<int, vector<GF<BIG_MOD>>> FF, FR, FH, FV;
	int vSize;
	int hSize;

	Toeplitz()
		: vSize(0), hSize(0)
	{
	}

	Toeplitz(const Toeplitz& other)
		: F(other.F), vSize(other.vSize), hSize(other.hSize)
	{
	}

	Toeplitz(Toeplitz&& other)
		: F(move(other.F)), vSize(other.vSize), hSize(other.hSize)
	{
	}

	Toeplitz& operator=(const Toeplitz& other)
	{
		if (this != &other) {
			F = other.F;
			vSize = other.vSize;
			hSize = other.hSize;
			FF = other.FF;
			FR = other.FR;
			FH = other.FH;
			FV = other.FV;
		}
		return *this;
	}

	Toeplitz& operator=(Toeplitz&& other)
	{
		F = move(other.F);
		vSize = other.vSize;
		hSize = other.hSize;
		FF = move(other.FF);
		FR = move(other.FR);
		FH = move(other.FH);
		FV = move(other.FV);
		return *this;
	}

	Toeplitz(const vector<int>& F_, int vSize_, int hSize_)
		: F(F_), vSize(vSize_), hSize(hSize_)
	{
		if (vSize <= 0 || hSize <= 0 || vSize + hSize - 1 != F.size()) {
			cerr << "invalid vector size for toeplitz matrix\n";
			exit(1);
		}
	}

	Toeplitz(vector<int>&& F_, int vSize_, int hSize_)
		: F(move(F_)), vSize(vSize_), hSize(hSize_)
	{
		if (vSize <= 0 || hSize <= 0 || vSize + hSize - 1 != F.size()) {
			cerr << "invalid vector size for toeplitz matrix\n";
			exit(1);
		}
	}

	Toeplitz(int vSize_, int hSize_, const int& v0)
		: F(vSize_ + hSize_ - 1), vSize(vSize_), hSize(hSize_)
	{
		if (vSize <= 0 || hSize <= 0) {
			cerr << "invalid vector size for toeplitz matrix\n";
			exit(1);
		}
		F[vSize - 1] = v0;
	}

	Toeplitz& operator+=(const Toeplitz& other)
	{
		if (hSize != other.hSize || vSize != other.vSize) {
			cerr << "inconsistent matrix sizes for sum\n";
			exit(1);
		}
		transform(F.begin(), F.end(), other.F.begin(), F.begin(), [](const int& x, const int& y) { return x + y; });
		return *this;
	}

	Toeplitz operator+(const Toeplitz& other)
	{
		if (hSize != other.hSize || vSize != other.vSize) {
			cerr << "inconsistent matrix sizes for sum\n";
			exit(1);
		}
		Toeplitz C(vSize, hSize, 0);
		transform(F.begin(), F.end(), other.F.begin(), C.F.begin(), [](const int& x, const int& y) { return x + y; });
		return C;
	}

	Toeplitz operator*(Toeplitz& other)
	{
		if (hSize != other.vSize) {
			cerr << "inconsistent matrix sizes for product\n";
			exit(1);
		}
		int limit = int(F.size());
		int bits = 0;
		while ((1 << bits) < limit) {
			++bits;
		}
		int sz = 1 << bits;
		auto V1 = convolution_IT(F.rbegin(), F.rend(), FR[sz], other.F.rbegin() + other.hSize - 1, other.F.rend(), other.FV[sz], bits, limit);
		limit = int(other.F.size());
		bits = 0;
		while ((1 << bits) < limit) {
			++bits;
		}
		sz = 1 << bits;
		auto H1 = convolution_IT(F.begin() + vSize - 1, F.end(), FH[sz], other.F.begin(), other.F.end(), other.FF[sz], bits, limit);
		int vSize2 = vSize;
		int hSize2 = other.hSize;
		vector<int> F2(vSize2 + hSize2 - 1);
		copy(V1.rbegin(), V1.rbegin() + vSize2, F2.begin());
		copy(H1.begin() + hSize - 1, H1.end(), F2.begin() + vSize2 - 1);
		return Toeplitz(move(F2), vSize2, hSize2);
	}

	Toeplitz& operator%=(int mod)
	{
		transform(F.begin(), F.end(), F.begin(), [=](const int& x) { return x % mod; });
		return *this;
	}

	void output() const
	{
		cout << "V:";
		for (auto it = F.rbegin() + hSize - 1; it != F.rend(); ++it) {
			cout << " " << unsigned_value(*it);
		}
		cout << ", H:";
		for (auto it = F.begin() + vSize - 1; it != F.end(); ++it) {
			cout << " " << unsigned_value(*it);
		}
	}
};

struct BlockToeplitz
{
	array<array<Toeplitz, 2>, 2> M;

	BlockToeplitz()
	{
	}

	BlockToeplitz(const BlockToeplitz& other)
		: M(other.M)
	{
	}

	BlockToeplitz(BlockToeplitz&& other)
		: M(move(other.M))
	{
	}

	BlockToeplitz& operator=(const BlockToeplitz& other)
	{
		if (this != &other) {
			M = other.M;
		}
		return *this;
	}

	BlockToeplitz& operator=(BlockToeplitz&& other)
	{
		M = move(other.M);
		return *this;
	}

	BlockToeplitz& operator+=(const BlockToeplitz& other)
	{
		M[0][0] += other.M[0][0];
		M[0][1] += other.M[0][1];
		M[1][0] += other.M[1][0];
		M[1][1] += other.M[1][1];
		return *this;
	}

	BlockToeplitz operator*(BlockToeplitz& other)
	{
		BlockToeplitz C;
		C.M[0][0] = M[0][0] * other.M[0][0] + M[0][1] * other.M[1][0];
		C.M[0][1] = M[0][0] * other.M[0][1] + M[0][1] * other.M[1][1];
		C.M[1][0] = M[1][0] * other.M[0][0] + M[1][1] * other.M[1][0];
		C.M[1][1] = M[1][0] * other.M[0][1] + M[1][1] * other.M[1][1];
		C.M[0][0] %= 119;
		C.M[0][1] %= 119;
		C.M[1][0] %= 119;
		C.M[1][1] %= 119;
		return C;
	}

	void output() const
	{
		for (int i1 = 0; i1 < 2; ++i1) {
			for (int j1 = 0; j1 < 2; ++j1) {
				auto& F = M[i1][j1].F;
				for (int i = M[i1][j1].vSize - 1; i < int(F.size()); ++i) {
					cout << unsigned_value(F[i]) << " ";
				}
				cout << "| ";
			}
			cout << "\n";
			for (int i = 1; i < int(M[i1][0].vSize); ++i) {
				for (int j1 = 0; j1 < 2; ++j1) {
					cout << signed_value(M[i1][j1].F[M[i1][0].vSize - 1 - i]) << " ";
					for (int j = 1; j < M[i1][j1].hSize; ++j) {
						cout << "\\ ";
					}
					cout << "| ";
				}
				cout << "\n";
			}
			cout << "---\n";
		}
	}
};

template<class T>
T solve_slowly(int n, int a, int b, int p, int q)
{
	if (n <= 0) return T(1);
	vector<T> V(n + 1);
	V[0] = T(1);
	for (int i = 1; i <= n; ++i) {
		if (i > p) {
			V[i] += V[i - p] * T(a);
		} else {
			V[i] += T(a);
		}
		if (i > q) {
			V[i] += V[i - q] * T(b);
		} else {
			V[i] += T(b);
		}
	}
	return V[n];
}

int solve_fast(int n, int a, int b, int p, int q, int mod)
{
	int s0 = p, s1 = q - p;
	vector<int> F00(s0 + s0 - 1), F01(s0 + s1 - 1), F10(s1 + s0 - 1), F11(s1 + s1 - 1);
	if (s0 > 1) {
		F00[s0 - 2] = 1;
	}
	if (s1 > 1) {
		F11[s1 - 2] = 1;
	}
	F00.back() = a % mod;
	F01.back() = b % mod;
	F10.back() = 1;
	BlockToeplitz BT;
	BT.M[0][0] = Toeplitz(move(F00), s0, s0);
	BT.M[0][1] = Toeplitz(move(F01), s0, s1);
	BT.M[1][0] = Toeplitz(move(F10), s1, s0);
	BT.M[1][1] = Toeplitz(move(F11), s1, s1);
	BlockToeplitz RT;
	RT.M[0][0] = Toeplitz(s0, s0, 1);
	RT.M[0][1] = Toeplitz(s0, s1, 0);
	RT.M[1][0] = Toeplitz(s1, s0, 0);
	RT.M[1][1] = Toeplitz(s1, s1, 1);
	while (n > 0) {
		if (n & 1) {
			RT = RT * BT;
			//cout << "RT:" << endl;
			//RT.output();
		}
		n >>= 1;
		if (n > 0) {
			BT = BT * BT;
		}
	}
	auto F0_begin = RT.M[0][0].F.begin() + RT.M[0][0].vSize - 1;
	auto F0_end = RT.M[0][0].F.end();
	auto F1_begin = RT.M[0][1].F.begin() + RT.M[0][1].vSize - 1;
	auto F1_end = RT.M[0][1].F.end();
	int result = accumulate(F0_begin, F0_end, 0);
	result = accumulate(F1_begin, F1_end, result);
	return result % mod;
}

void stress()
{
	for (;;) {
		int p = 1 + rand() % 100;
		int q = p + 1 + rand() % 100;
		auto a = rand() % 119;
		auto b = rand() % 119;
		int n = rand();
		cout << "n:" << n << " a:" << a << " b:" << b << " p:" << p << " q:" << q << endl;
		int r = solve_fast(n, a, b, p, q, 119);
		cout << "fast r:" << r << endl;
		int r2 = *solve_slowly<GF<119>>(n, a, b, p, q);
		cout << "slow r:" << r2 << endl;
		if (r != r2) {
			cout << "failed" << endl;
			exit(1);
		}
	}
}

void time_stress()
{
	for (;;) {
		int p = 1 + rand() % 9999;
		int q = 10000;
		auto a = rand() % 119;
		auto b = rand() % 119;
		int n = 2'000'000'000;
		cout << "n:" << n << " a:" << a << " b:" << b << " p:" << p << " q:" << q << endl;
		int r = solve_fast(n, a, b, p, q, 119);
		cout << "fast r:" << r << endl;
	}
}

void test1()
{
	Toeplitz A(3, 2, 0);
	A.F[0] = 1; A.F[1] = 2; A.F[2] = 3; A.F[3] = 4;
	Toeplitz B(2, 3, 0);
	B.F[0] = 5; B.F[1] = 6; B.F[2] = 7; B.F[3] = 8;
	A.output();
	cout << endl;
	B.output();
	cout << endl;
	auto C = A * B;
	C.output();
	cout << endl;
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	ntt_prepare(20);

	//test1();
	//stress();
	//time_stress();

	int n, a, b, p, q;
	cin >> n >> a >> b >> p >> q;
	cout << solve_fast(n, a, b, p, q, 119) << endl;
	return 0;
}
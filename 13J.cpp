#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <numeric>
#include <cmath>
#include <cassert>

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


template<int MOD>
GF<MOD> min_primitive_root(int PHI)
{
	using T = GF<MOD>;
	vector<int> factors;
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

int signed_value(const int& v)
{
	return v;
}

template<int MOD>
int signed_value(const GF<MOD>& v)
{
	return v.signed_value();
}

template<class T1, class T2>
vector<T2> module_transform(const vector<T1>& A)
{
	vector<T2> B(A.size());
	transform(A.begin(), A.end(), B.begin(), [](const T1& a) {
		return T2(signed_value(a));
	});
	return B;
}

template<class T>
vector<T> convolution(vector<T> A, vector<T> B)
{
	int limit = int(A.size() + B.size() - 1);
	int sz = 1;
	while (sz < limit) {
		sz *= 2;
	}
	A.resize(sz);
	B.resize(sz);
	ntt(A);
	ntt(B);
	transform(A.begin(), A.end(), B.begin(), A.begin(), [](const T& a, const T& b) { return a * b; });
	intt(A);
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

template<class T>
vector<T> convolution_BIG_MOD(const vector<T>& A1, const vector<T>& B1)
{
	auto A = module_transform<T, GF<BIG_MOD>>(A1);
	auto B = module_transform<T, GF<BIG_MOD>>(B1);
	int sz = 1;
	while (sz < int(A.size() + B.size() - 1)) {
		sz *= 2;
	}
	A.resize(sz);
	B.resize(sz);
	ntt(A);
	ntt(B);
	transform(A.begin(), A.end(), B.begin(), A.begin(), [](const GF<BIG_MOD>& a, const GF<BIG_MOD>& b) { return a * b; });
	intt(A);
	auto C1 = module_transform<GF<BIG_MOD>, T>(A);
	C1.resize(A1.size() + B1.size() - 1);
	return C1;
}

template<class T>
vector<T> tribonacci(int n, int k, const vector<T>& Dnm1, const vector<T>& Dnm2)
{
	int limit = min(n + 1, k + 1);
	vector<T> Dn(limit);
	Dn[0] = T(1);
	for (int m = 1; m < limit && m < int(Dnm1.size()); ++m) {
		Dn[m] = Dnm1[m] + Dnm1[m - 1] + Dnm2[m - 1];
	}
	if (limit == n + 1) {
		Dn[n] = T(1);
	}
	return Dn;
}

template<class T>
pair<vector<T>, vector<T>> solve(int n, int k)
{
	int limit = min(n + 1, k + 1);
	int limit_m1 = min(n, k + 1);
	vector<T> Dn(limit), Dnm1(limit_m1);
	if (n == 0) {
		Dn[0] = T(1);
		return { Dnm1, Dn };
	} else if (n == 1) {
		Dnm1[0] = Dn[0] = Dn[1] = T(1);
		return { Dnm1, Dn };
	}
	vector<T> A; // D((n-1)/2-1,m)
	vector<T> B; // D((n-1)/2,m)
	vector<T> C; // D((n-1)/2+1,m)
	tie(A, B) = solve<T>((n - 1) / 2, k);
	C = tribonacci((n - 1) / 2 + 1, k, B, A);
	auto BC = convolution(B, C);
	auto AB = convolution(A, B);
	auto BB = convolution(B, B);
	auto sum = [](const T& x, const T& y) { return x + y; };
	if (n % 2 == 1) {
		auto AA = convolution(A, A);
		copy(BC.begin(), BC.begin() + min(int(BC.size()), limit), Dn.begin());
		transform(AB.begin(), AB.begin() + min(int(AB.size()), limit - 1), Dn.begin() + 1, Dn.begin() + 1, sum);
		copy(BB.begin(), BB.begin() + min(int(BB.size()), limit), Dnm1.begin());
		transform(AA.begin(), AA.begin() + min(int(AA.size()), limit - 1), Dnm1.begin() + 1, Dnm1.begin() + 1, sum);
	} else {
		auto CC = convolution(C, C);
		copy(CC.begin(), CC.begin() + min(int(BC.size()), limit), Dn.begin());
		transform(BB.begin(), BB.begin() + min(int(BB.size()), limit - 1), Dn.begin() + 1, Dn.begin() + 1, sum);
		copy(BC.begin(), BC.begin() + min(int(BC.size()), limit), Dnm1.begin());
		transform(AB.begin(), AB.begin() + min(int(AB.size()), limit - 1), Dnm1.begin() + 1, Dnm1.begin() + 1, sum);
	}
	if (limit == n + 1) {
		Dn[n] = T(1);
	}
	if (limit_m1 == n) {
		Dnm1[n - 1] = T(1);
	}
	return { Dnm1, Dn };
}

template<class T>
void test()
{
	for (int n = 1; n <= 15; ++n) {
		vector<T> Dnm1, Dn;
		tie(Dnm1, Dn) = solve<T>(n, 6);
		cout << "n:" << n << endl;
		for (auto& v : Dn) {
			cout << *v << " ";
		}
		cout << endl;
	}
	for (int n = 1'000'000'000; n <= 1'000'000'002; ++n) {
		vector<T> Dnm1, Dn;
		tie(Dnm1, Dn) = solve<T>(n, 6);
		cout << "n:" << n << endl;
		for (auto& v : Dn) {
			cout << *v << " ";
		}
		cout << endl;
	}
}

int main()
{
	using T = GF<BIG_MOD>;
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	//test<T>();
	int n, k;
	cin >> n >> k;
	vector<T> Dnm1, Dn;
	tie(Dnm1, Dn) = solve<T>(n, k);
	for (int m = 1; m <= k; ++m) {
		if (m < int(Dn.size())) {
			cout << *Dn[m] << " ";
		} else {
			cout << "0 ";
		}
	}
	cout << "\n";
	return 0;
}
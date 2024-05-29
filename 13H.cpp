#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

constexpr int MOD = 7'340'033;

int gcd(int a, int b)
{
	return !b ? a : gcd(b, a % b);
}

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
		v = (v - other.v + MOD) % MOD;
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
};

GF operator""_r(unsigned long long v)
{
	return GF(v % MOD);
}

constexpr int PHI = MOD - 1;
GF G(3);

vector<GF> ROOTS = {
	7340032_r, 2306278_r, 2001861_r, 1627581_r, 3353052_r,
	3114847_r, 2515483_r, 1357781_r,  680314_r, 3513903_r,
	1736463_r,  153766_r, 2743563_r,  907050_r, 1504761_r,
	2903335_r, 1434658_r,  338192_r,  746933_r, 3478878_r,
}; // корни степеней 2^{k+1} для 1

vector<GF> REVERSED_ROOTS = {
	7340032_r, 5033755_r, 3413510_r, 4961214_r, 7295050_r,
	6067369_r, 5416304_r, 6777965_r, 3629816_r, 1947550_r,
	1711230_r, 3489196_r, 2046946_r, 6341884_r, 585335_r,
	431801_r, 2653924_r, 3970201_r, 5391104_r, 5020900_r,
}; // корни степеней -2^{k+1} для 1

GF min_primitive_root()
{
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
			if (GF(g).power(PHI / factors[i]) == GF(1)) {
				flag = false;
				break;
			}
		}
		if (flag) {
#ifdef _DEBUG
			cerr << "primitive_root = " << g << "\n";
#endif
			return GF(g);
		}
	}
#ifdef _DEBUG
	cerr << "no primitive root was found\n";
#endif
	return GF();
}

vector<int> discrete_log(GF a, GF b)
{
	vector<int> ans;
	int sq = int(sqrt(double(MOD))) + 1;
	auto asq = a.power(sq);
	map<GF, int> values;
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

vector<GF> discrete_root(int k, GF a)
{
	if (a == GF(0)) {
		return { a };
	}
	auto log_ans = discrete_log(G.power(k), a);
	if (log_ans.empty()) {
		return {};
	}
	int y = log_ans[0];
	int delta = PHI / gcd(PHI, k);
	vector<GF> ans;
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

void precalculate()
{
	assert(min_primitive_root() == G);
	GF cur(1);
	vector<GF> roots;
	while (true) {
		auto v = discrete_root(2, cur);
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

int reverse_bits(int n, int bits)
{
	int v = 0;
	for (int i = 0; i < bits; ++i) {
		v = (v << 1) | (n & 1);
		n >>= 1;
	}
	return v;
}

void ntt(vector<GF>& a, const vector<GF>& roots = ROOTS)
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
		GF e = roots[bits]; // 1^(1/bs)
		for (int off = 0; off < N; off += bs) { // сдвиг блока
			GF f(1); // e^j == 1^(j/bs)
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

void intt(vector<GF>& a, const vector<GF>& roots = REVERSED_ROOTS)
{
	ntt(a, roots);
	GF m = ~GF(a.size());
	transform(a.begin(), a.end(), a.begin(), [&](const GF& x) { return x * m; });
}

void output(const vector<GF>& a, const char* comment = "")
{
	cout << comment;
	for (auto& v : a) {
		cout << *v << " ";
	}
	cout << "\n";
}

vector<GF> convolution(const vector<GF>& A, const vector<GF>& B)
{
	vector<GF> C(A.size() + B.size() - 1);
	for (int i = 0; i < int(A.size()); ++i) {
		for (int j = 0; j < int(B.size()); ++j) {
			C[i + j] += A[i] * B[j];
		}
	}
	return C;
}

vector<GF> poly_fast_product(vector<GF> A, vector<GF> B)
{
	ntt(A);
	ntt(B);
	transform(A.begin(), A.end(), B.begin(), A.begin(), [](const GF& a, const GF& b) { return a * b; });
	intt(A);
	return A;
}

void poly_mult_by(vector<GF>& A, const GF& y)
{
	transform(A.begin(), A.end(), A.begin(), [&](const GF& x) { return x * y; });
}

void poly_add_with(vector<GF>& A, const vector<GF>& B)
{
	transform(A.begin(), A.end(), B.begin(), A.begin(), [](const GF& a, const GF& b) { return a + b; });
}

void poly_enlarge(vector<GF>& A, int k)
{
	copy(A.rbegin() + k, A.rend(), A.rbegin());
	fill(A.begin(), A.begin() + k, GF(0));
}

void poly_reduce(vector<GF>& A, int k)
{
	copy(A.begin() + k, A.end(), A.begin());
	fill(A.rbegin(), A.rbegin() + k, GF(0));
}

vector<GF> poly_reciprocal(const vector<GF>& P)
{
	int sz = int(P.size()); // sz - степень двойки
	if (sz == 1) {
		return { ~P[0] };
	}
	auto Q = poly_reciprocal(vector<GF>(P.begin() + sz / 2, P.end()));
	auto R = P;
	poly_mult_by(R, GF(-1));
	R.resize(sz * 2);
	Q.resize(sz * 2);
	R = poly_fast_product(R, Q);
	R = poly_fast_product(R, Q);
	poly_enlarge(Q, sz * 3 / 2 - 2); // * x^(3/2*sz-2)
	poly_mult_by(Q, GF(2));
	poly_add_with(R, Q);
	poly_reduce(R, sz - 2); // / x^(sz-2)
	R.resize(sz);
	return R;
}

GF poly_eval(const vector<GF>& poly, GF x)
{
	GF res;
	for (auto it = poly.rbegin(); it != poly.rend(); ++it) {
		res *= x;
		res += *it;
	}
	return res;
}

void ntt_test()
{
	precalculate();
	int N = 4;
	vector<GF> P(N + 1);
	P[0] = 1; P[1] = 1; P[2] = -1; P[3] = 2; P[4] = 3;
	int sz = 1; // размер матрицы для БПФ
	while (sz < N + 1) {
		sz *= 2;
	}
	P.resize(sz);
	vector<int> S(sz);
	auto P0 = P;
	ntt(P, ROOTS);
	output(P, "after NTT = ");
	intt(P, REVERSED_ROOTS);
	output(P, "after INTT = ");
	cout << "poly_eval(1) = " << *poly_eval(P0, GF(1)) << " " << *poly_eval(P, GF(1)) << "\n";
	cout << "poly_eval(ROOTS[2]) = " << *poly_eval(P0, ROOTS[2]) << " " << *poly_eval(P, ROOTS[2]) << "\n";
	cout << "poly_eval(ROOTS[1]) = " << *poly_eval(P0, ROOTS[1]) << " " << *poly_eval(P, ROOTS[1]) << "\n";
	cout << "poly_eval(ROOTS[2]^3) = " << *poly_eval(P0, ROOTS[2].power(3)) << " " << *poly_eval(P, ROOTS[2].power(3)) << "\n";
	cout << "poly_eval(ROOTS[0]) = " << *poly_eval(P0, ROOTS[0]) << " " << *poly_eval(P, ROOTS[0]) << "\n";
	cout << "poly_eval(ROOTS[2]^5) = " << *poly_eval(P0, ROOTS[2].power(5)) << " " << *poly_eval(P, ROOTS[2].power(5)) << "\n";
	cout << "poly_eval(ROOTS[1]^3) = " << *poly_eval(P0, ROOTS[1].power(3)) << " " << *poly_eval(P, ROOTS[1].power(3)) << "\n";
	cout << "poly_eval(ROOTS[2]^7) = " << *poly_eval(P0, ROOTS[2].power(7)) << " " << *poly_eval(P, ROOTS[2].power(7)) << "\n";
}

vector<GF> solve(vector<GF> P, int M)
{
	if (P[0] == GF(0)) {
		return {};
	}
	P.resize(M); // члены старше x^M выкидываем
	int sz = 1;
	while (sz < M) {
		sz *= 2;
	}
	P.resize(sz); // увеличиваем степень P до степени 2 минус 1
	reverse(P.begin(), P.end()); // отражаем многочлен
	auto Q = poly_reciprocal(P); // вычисляем частное от деления x^(2*sz-2) на P
	reverse(Q.begin(), Q.end()); // отражаем многочлен
	Q.resize(M); // члены старше x^M выкидываем
	return Q;
}

void test(const vector<GF>& P, int M)
{
	auto Q = solve(P, M);
	if (P[0] == GF(0)) {
		if (Q.empty()) {
			cout << "test failed: expect empty Q\n";
			cout << "M = " << M << "\n";
			output(P, "P: ");
			output(Q, "Q: ");
			exit(1);
		}
	} else {
		if (Q.size() > M) {
			cout << "test failed: invalid Q size\n";
			cout << "M = " << M << "\n";
			output(P, "P: ");
			output(Q, "Q: ");
			exit(1);
		}
		vector<GF> R;
		if (P.size() < 100 && Q.size() < 100) {
			R = convolution(P, Q);
		} else {
			int sz = 1;
			while (sz < int(P.size() + Q.size()) + 1) {
				sz *= 2;
			}
			auto P1 = P;
			P1.resize(sz);
			Q.resize(sz);
			R = poly_fast_product(P1, Q);
		}
		bool ok = true;
		for (int i = 0; i < M; ++i) {
			if (*R[i] != !i) {
				ok = false;
			}
		}
		if (!ok) {
			cout << "test failed: invalid convolution\n";
			cout << "M = " << M << "\n";
			output(P, "P: ");
			output(Q, "Q: ");
			output(R, "R: ");
			exit(1);
		}
		if (P.size() < 10 and Q.size() < 10) {
			output(P, "P: ");
			output(Q, "Q: ");
			output(R, "R: ");
		} else {
			cout << "N: " << P.size() << " M:" << M << "\n";
		}
	}
}

void stress()
{
	for (;;) {
		int N = rand() % 10 + 1;
		int M = rand() % 10 + 1;
		vector<GF> P(N + 1);
		for (auto& p : P) {
			p = GF(rand());
		}
		test(P, M);
	}
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int M, N;
	cin >> M >> N;
	vector<GF> P(N + 1);
	for (auto& a : P) {
		int v;
		cin >> v;
		a = v;
	}
	auto Q = solve(P, M);
	if (Q.empty()) {
		cout << "The ears of a dead donkey\n";
	} else {
		output(Q);
		// auto R = convolution(P, Q);
		// output(R, "check: ");
	}
	return 0;
}
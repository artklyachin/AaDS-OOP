#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

constexpr int MOD = 1e9 + 7;

int power(int n, int k)
{
    n %= MOD;
    k %= MOD - 1;
    int r = 1, m = n;
    while (k != 0) {
        if (k & 1) {
            r = 1LL * r * m % MOD;
        }
        m = 1LL * m * m % MOD;
        k /= 2;
    }
    return r;
}

int rev(int n)
{
    return power(n, MOD - 2);
}

vector<int> F, R;

int factorial(int n)
{
    if (F.empty()) {
        F.push_back(1);
    }
    for (int k = int(F.size()); k <= n; ++k) {
        F.push_back(1LL * F.back() * k % MOD);
    }
    return F[n];
}

int rev_factorial(int n)
{
    if (R.size() < n + 1) {
        R.resize(n + 1);
    }
    if (R[n] == 0) {
        R[n] = rev(factorial(n));
    }
    return R[n];
}

int binomial(int n, int m)
{
    if (n < 0 || m < 0 || m > n) {
        return 0;
    }
    return 1LL * factorial(n) * rev_factorial(m) % MOD * rev_factorial(n - m) % MOD;
}

int stirling2(int n, int m)
{
    if (n == 0 && m == 0) {
        return 1;
    } else if (n < 1 || m < 1 || m > n) {
        return 0;
    }
    int s = 0;
    for (int k = 0; k <= m; ++k) {
        int t = 1LL * binomial(m, k) * power(k, n) % MOD;
        if ((m - k) & 1) {
            t = MOD - t;
        }
        s = (s + t) % MOD;
    }
    return 1LL * s * rev_factorial(m) % MOD;

}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K;
    cin >> N >> K;
    F.reserve(N + 1);
    R.reserve(N + 1);
    int S = 0;
    for (int n = 0; n < N; ++n) {
        int v;
        cin >> v;
        S = (S + v) % MOD;
    }
    int ans = 1LL * (N - 1) * stirling2(N - 1, K) % MOD;
    ans = (ans + stirling2(N, K)) % MOD;
    ans = 1LL * ans * S % MOD;
    cout << ans << "\n";
    return 0;
}
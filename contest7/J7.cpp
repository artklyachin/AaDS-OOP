#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <random>
#include <assert.h>
#include <algorithm>


using namespace std;


const int MOD = 999'999'937;


vector<vector<int64_t>> mulmod(const vector<vector<int64_t>>& a, const vector<vector<int64_t>>& b)
{
    int64_t N = int64_t(a.size());
    vector<vector<int64_t>> c(N, vector<int64_t>(N));
    for (int64_t i = 0; i < N; ++i) {
        for (int64_t j = 0; j < N; ++j) {
            for (int64_t k = 0; k < N; ++k) {
                c[i][k] = (c[i][k] + a[i][j] * b[j][k] % MOD) % MOD;
            }
        }
    }
    return c;
}


vector<vector<int64_t>> powmod(const vector<vector<int64_t>>& a, int64_t b)
{
    int64_t N = int64_t(a.size());
    vector<vector<int64_t>> c(N, vector<int64_t>(N));
    for (int64_t i = 0; i < N; ++i) {
        c[i][i] = 1;
    }
    vector<vector<int64_t>> p = a;
    while (b) {
        if (b & 1) {
            c = mulmod(c, p);
        }
        p = mulmod(p, p);
        b = b >> 1;
    }
    return c;
}


class sequence {
    int N;
    vector<vector<int64_t>> a;
public:
    sequence(const vector<vector<int64_t>>& m, int N)
        : N(N)
    {
        a = powmod(m, N - 1);
    }

    int number_of_lines()
    {
        int64_t ans = 0;
        for (int64_t i = 0; i < 5; ++i) {
            for (int64_t j = 0; j < 5; ++j) {
                ans += a[i][j];
                ans %= MOD;
            }
        }
        return ans;
    }

};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t N;
    vector<vector<int64_t>> m(5, vector<int64_t>(5, 1LL));
    m[2][3] = 0LL;
    m[2][4] = 0LL;
    m[4][3] = 0LL;
    m[4][4] = 0LL;
    while (true) {
        cin >> N;
        if (N <= 0) {
            return 0;
        }
        if (N == 1LL) {
            cout << 5 << endl;
            continue;
        }
        sequence seq(m, N);
        cout << seq.number_of_lines() << endl;
    }
    return 0;
}

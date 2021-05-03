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


int MOD = 1e9 + 7;


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


vector<vector<int64_t>> powmod(const vector<vector<int64_t>>& a, vector<int64_t>& b)
{
    int64_t N = int64_t(a.size());
    vector<vector<int64_t>> c(N, vector<int64_t>(N));
    for (int64_t i = 0; i < N; ++i) {
        c[i][i] = 1;
    }
    vector<vector<int64_t>> p = a;
    for (int i = 0; i < b.size(); ++i) {
        if (b[i] & 1) {
            c = mulmod(c, p);
        }
        p = mulmod(p, p);
    }
    return c;
}


class Board {
    uint64_t maxmask;
    vector<vector<int64_t>> a;
public:
    Board(int M)
    {
        maxmask = (1LL << M);
        a.resize(maxmask, vector<int64_t>(maxmask, 1));
        get_transition_matrix(M);
    }

    void get_transition_matrix(int M)
    {
        for (int64_t i = 0; i < maxmask; ++i) {
            for (int64_t j = 0; j < maxmask; ++j) {
                int64_t i1 = i, j1 = j;
                int64_t p1 = i1 & 1, p2 = j1 & 1;
                for (int64_t q = 1; q < M; ++q) {
                    i1 = i1 >> 1, j1 = j1 >> 1;
                    int64_t c1 = i1 & 1, c2 = j1 & 1;
                    if (p1 == p2 && p2 == c1 && c1 == c2) {
                        a[i][j] = 0;
                        break;
                    }
                    p1 = c1;
                    p2 = c2;
                }
            }
        }
    }

    void subtraction_of_one(string s, vector<int64_t>& nums, int len, int digit)
    {
        for (int i = 0; i < s.size() / len + 1; ++i) { // вычитаю 1
            if (nums[i] - 1 >= 0) {
                nums[i] -= 1;
                break;
            } else {
                nums[i] = digit - 1;
            }
        }
    }

    void binary_representation(string s, vector<int64_t>& nums, vector<int64_t> &d, int len, int digit)
    {
        while (nums.size() > 0) { // получаю двоичное представление
            d.push_back(nums[0] & 1);
            nums[0] /= 2;
            for (int i = 1; i < nums.size(); ++i) {
                nums[i - 1] += nums[i] * digit / 2;
                nums[i] = nums[i - 1] / digit;
                nums[i - 1] %= digit;
            }
            while (nums.size() > 0 && *nums.rbegin() == 0) {
                nums.pop_back();
            }
        }
    }

    int number_of_coloring_board(string s)
    {
        vector<vector<int64_t>> res(maxmask, vector<int64_t>(maxmask, 0));
        for (int i = 0; i < maxmask; ++i) {
            res[i][i] = 1;
        }
        reverse(s.begin(), s.end());
        vector<vector<int64_t>> pow = a;
        vector<int64_t> nums;
        int len = 9, digit = 1e9;
        for (int i = 0; i < s.size() / len + 1; ++i) {
            int num = 0, st = 1;
            for (int j = i * len; j < (i + 1) * len && j < s.size(); ++j) {
                num += (s[j] - '0') * st;
                st *= 10;
            }
            nums.push_back(num);
        }
        subtraction_of_one(s, nums, len, digit);
        vector<int64_t> d;
        binary_representation(s, nums, d, len, digit);
        res = powmod(a, d);
        int64_t ans = 0;
        for (int64_t i = 0; i < maxmask; ++i) {
            for (int64_t j = 0; j < maxmask; ++j) {
                ans += res[i][j];
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
    string s;
    int64_t M, mod;
    cin >> s >> M >> mod;
    MOD = mod;
    if (MOD == 1) {
        cout << 0 << endl;
        return 0;
    }
    Board board(M);
    cout << board.number_of_coloring_board(s) << endl;
    return 0;
}


//45376357846376 6 6

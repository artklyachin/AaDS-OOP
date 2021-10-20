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


class Find_Equels
{
private:
    vector<int> ans;
    int64_t MOD = 1e9 + 7;

public:


    Find_Equels(string s, string t)
    {
        int64_t hash_s = 0, hash_t = 0, r = 5, power_r = 1;
        for (int i = 0; i < t.size(); ++i) {
            hash_t += power_r * (t[i] - 'a');
            hash_s += power_r * (s[i] - 'a');
            power_r *= r;
            hash_t %= MOD;
            hash_s %= MOD;
            power_r %= MOD;
        }

        vector<int64_t> h(s.size());
        int64_t st_r = 1;
        for (int i = 0; i < s.size(); ++i) {
            h[i] = (s[i] - 'a') * st_r % MOD;
            st_r *= r;
            st_r %= MOD;
        }

        for (int j = t.size() - 1; j < s.size(); ++j) {
            if (hash_s == hash_t) {
                ans.push_back(j - t.size() + 1);
            }
            if (j == s.size() - 1) {
                break;
            }
            hash_s += MOD - h[j - t.size() + 1];
            hash_s %= MOD;
            hash_s += h[j + 1];
            hash_s %= MOD;
            hash_t *= r;
            hash_t %= MOD;
        }
    }

    void Print_equels()
    {
        for (int i : ans) {
            cout << i << " ";
        }
        cout << endl;
    }

};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s, t;
    cin >> s >> t;
    Find_Equels elem(s, t);
    elem.Print_equels();
    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map> 
#include <cmath>         
#include <algorithm>

using namespace std;

struct SuffArray {

    SuffArray(string s)
    {
        int n = s.size();
        s.push_back('$');
        int logorithm = int(ceil(log2(n + 1)));
        int h = 1 << logorithm;
        s.resize(h, '$');
        vector<int> p = get_primary_p(s), color(h);
        for (int i = 1; i < h; ++i) {
            color[i] = color[i - 1];
            if (s[p[i - 1]] != s[p[i]]) {
                ++color[i];
            }
        }

        for (int pow = 1; pow < h; pow *= 2) {
            vector<int> p2(h), pnext(h);
            for (int j = 0; j < h; ++j) {
                p2[j] = p[j] - pow;
                if (p2[j] < 0) {
                    p2[j] += h;
                }
            }
            vector<int> count(color.size());
            for (int j = 0; j < h; ++j) {
                ++count[color[j]];
            }
            for (int j = 1; j < color.size(); ++j) {
                count[j] += count[j - 1];
            }
            vector<int> color_p(h);
            for (int j = 0; j < h; ++j) {
                color_p[p[j]] = color[j];
            }
            vector<pair<int, int>> color2(h);
            for (int j = h - 1; j >= 0; --j) {
                int new_index = --count[color_p[p2[j]]];
                pnext[new_index] = p2[j];
                color2[new_index] = { color_p[p2[j]] , color[j] };
            }
            for (int j = 1; j < h; ++j) {
                if (color2[j - 1] != color2[j]) {
                    color[j] = color[j - 1] + 1;
                } else {
                    color[j] = color[j - 1];
                }
            }
            p = pnext;
        }
        suff_array.resize(n);
        copy(p.begin() + h - n, p.end(), suff_array.begin());
    }

    vector<int> getArray()
    {
        return suff_array;
    }

private:
    vector<int> suff_array;

    vector<int> get_primary_p(string s)
    {
        int n = s.size();
        vector<pair<char, int>> v(n);
        for (int i = 0; i < n; ++i) {
            v[i] = { s[i], i };
        }
        sort(v.begin(), v.end());
        vector<int> p(n);
        for (int i = 0; i < n; ++i) {
            p[i] = v[i].second;
        }
        return p;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s;
    cin >> s;
    SuffArray suff_array(s);
    vector<int> ans = suff_array.getArray();
    for (auto it = ans.begin(); it < ans.end(); ++it) {
        cout << *it + 1 << " ";
    }
    cout << endl;
    return 0;
}
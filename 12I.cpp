#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map> 
#include <cmath>         
#include <algorithm>
#include <map>

using namespace std;

struct Solution
{
    struct Section
    {
        int begin = -1;
        int end = -1;
        int next = -1;

        int len()
        {
            return end - begin;
        }
    };

    struct Node
    {
        int begin = 0, end = 0, parent = 0, id = 0;
        Node(int b, int e, int p) : begin(b), end(e), parent(p), id(0) {}
    };

    vector<pair<int, int>> M; // depth, nodeId
    vector<Node> T;
    vector<vector<int>> G;
    int order = 0;

    void dfs(int u)
    {
        T[u].id = order++;
        for (int v : G[u]) {
            cout << T[u].id << " " << T[v].begin << " " << T[v].end << "\n";
            dfs(v);
        }
    }

    Solution(const vector<int>& suf, const vector<int>& lcp)
    {
        int n = suf.size();
        T.reserve(2 * n + 10);
        T.emplace_back(0, 0, -1);
        T.emplace_back(suf[0], n, 0);
        M.emplace_back(0, 0);
        M.emplace_back(n - suf[0], 1);
        for (int i = 1; i < n; ++i) {
            int d = lcp[i - 1];
            auto it = lower_bound(M.begin(), M.end(), make_pair(d, 0));
            if (it->first == d) {
                int parent = it->second;
                T.emplace_back(suf[i] + d, n, parent);
                M.erase(it + 1, M.end());
            } else {
                int parent = prev(it)->second;
                int k = it->second;
                int m = d - prev(it)->first;
                T.emplace_back(T[k].begin, T[k].begin + m, parent);
                T[k].begin += m;
                T[k].parent = T.size() - 1;
                T.emplace_back(suf[i] + d, n, T.size() - 1);
                M.erase(it, M.end());
                M.emplace_back(d, T.size() - 2);
            }
            M.emplace_back(n - suf[i], T.size() - 1);
        }
        G.resize(T.size());
        for (int i = 1; i < int(T.size()); ++i) {
            G[T[i].parent].push_back(i);
        }
        cout << T.size() << "\n";
        dfs(0);
    }
};

struct SuffArray {

    SuffArray(string s)
    {
        int n = s.size();
        s.push_back('#');
        int logorithm = int(ceil(log2(n + 1)));
        int h = 1 << logorithm;
        s.resize(h, '#');
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

vector<int> Lcp(const string& s, vector<int>& ans)
{
    int n = s.size();
    vector<int> pos(n);
    for (int i = 0; i < n; ++i) {
        pos[ans[i]] = i;
    }
    vector<int> lcp(n - 1);
    int k = 0;
    for (int w = 0; w < n; ++w) {
        int i = pos[w];
        if (i == n - 1) {
            k = 0;
            continue;
        }
        int j = ans[i + 1];
        k = max(k - 1, 0);
        while (w + k < n && j + k < n && s[w + k] == s[j + k]) {
            ++k;
        }
        lcp[i] = k;
    }
    return lcp;
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s;
    cin >> s;
    SuffArray suff_array(s);
    vector<int> ans = suff_array.getArray();
#if 0
    for (int v : ans) {
        cout << v + 1 << " ";
    }
    cout << endl;
#endif
    auto lcp = Lcp(s, ans);
#if 0
    for (int i = 0; i < int(s.size() - 1); ++i) {
        cout << s.substr(ans[i]) << " " << s.substr(ans[i + 1]) << " " << lcp[i] << "\n";
    }
#endif
    Solution(ans, lcp);
    return 0;
}
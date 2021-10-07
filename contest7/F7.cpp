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


class graph {
private:
    vector<vector<int>> g;

public:
    graph(vector<vector<int>>& v)
    {
        g = v;
    }

    ~graph()
    {
    }

    vector<int> find_shortest_way()
    {
        int n = g.size();
        vector<vector<int>> a(1 << n, vector<int>(n, 1e9));
        vector<vector<pair<int, int>>> prev(1 << n, vector<pair<int, int>>(n, { 1e9, -1 }));
        for (int i = 0; i < n; ++i) {
            a[1 << i][i] = 0;
        }
        for (int mask = 0; mask < (1 << n); ++mask) {
            for (int i = 0; i < n; ++i) {
                if ((mask ^ (1 << i)) != mask) {
                    for (int j = 0; j < n; ++j) {
                        if ((mask ^ (1 << j)) != mask && j != i) {
                            if (a[mask][i] > a[mask ^ (1 << i)][j] + g[i][j]) {
                                a[mask][i] = a[mask ^ (1 << i)][j] + g[i][j];
                                prev[mask][i] = { mask ^ (1 << i) , j };
                            }
                        }
                    }
                }
            }
        }
        return restoration_of_response(a, prev);
    }

    vector<int> restoration_of_response(vector<vector<int>>& a, vector<vector<pair<int, int>>>& prev)
    {
        int n = g.size();
        int bestj = 0;
        for (int j = 0; j < n; ++j) {
            if (a[(1 << n) - 1][j] < a[(1 << n) - 1][bestj]) {
                bestj = j;
            }
        }
        vector<int> ans;
        int mask = (1 << n) - 1, j = bestj;
        cout << a[mask][j] << endl;
        while (j != -1) {
            ans.push_back(j);
            int newmask, newj;
            newmask = prev[mask][j].first;
            newj = prev[mask][j].second;
            mask = newmask;
            j = newj;
        }
        return ans;
    }

};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    vector<vector<int>> v(n, vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> v[i][j];
        }
    }
    graph g(v);
    vector<int> ans = g.find_shortest_way();
    for (int i : ans) {
        cout << i + 1 << " ";
    }
    cout << endl;
    return 0;
}

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


struct IGraph {
    virtual ~IGraph() = default;
};


class graph : public IGraph
{
private:
    vector<unordered_set<int>> tree;

public:
    graph(int N)
    {
        tree.resize(N);
    }

    ~graph()
    {
    }


    void connect(int a, int b)
    {
        tree[a].insert(b);
    }


    void dfs(vector<int>& tin, vector<int>& color, int& time, int v, int prev, vector<int>& ans)
    {
        int c = 0;
        bool flag = true;
        tin[v] = time;
        color[v] = time;
        ++time;
        for (int i : tree[v]) {
            if (i == prev) continue;
            if (tin[i] != -1) {
                color[v] = min(color[v], tin[i]);
            } else {
                dfs(tin, color, time, i, v, ans);
                color[v] = min(color[v], color[i]);
                if (prev != -1 && color[i] >= tin[v] && flag) {
                    ans.push_back(v + 1);
                    flag = false;
                }
                if (prev == -1) {
                    c += 1;
                }
            }
        }
        if (c >= 2 && prev == -1) {
            ans.push_back(v + 1);
        }
    }

    vector<int> articulation_points()
    {
        int n = tree.size();
        vector<int> tin(n, -1), color(n, -1), ans;
        int time = 0;
        for (int i = 0; i < n; ++i) {
            if (tin[i] == -1) {
                dfs(tin, color, time, i, -1, ans);
            }
        }
        sort(ans.begin(), ans.end());
        return ans;
    }

};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    cin >> n >> m;
    graph g(n);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u;
        --v;
        g.connect(u, v);
        g.connect(v, u);
    }
    vector<int> ans = g.articulation_points();
    cout << ans.size() << endl;
    for (int i : ans) {
        cout << i << " ";
    }
    cout << endl;
    return 0;
}

/*
7 9
1 2
2 3
3 1
4 5
4 3
5 3
6 3
7 3
6 7
*/
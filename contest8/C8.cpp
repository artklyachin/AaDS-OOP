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


    void dfs(vector<int>& visited, vector<int>& ans, int v, bool& flag, int& coincidence)
    {
        visited[v] = 1;
        for (int i : tree[v]) {
            if (visited[i] == 1) {
                coincidence = i;
                flag = true;
            } else if (visited[i] == 0) {
                dfs(visited, ans, i, flag, coincidence);
            }
            if (coincidence != -1) {
                if (flag) {
                    ans.push_back(v + 1);
                }
                if (coincidence == v) {
                    flag = false;
                }
                return;
            }
        }
        visited[v] = 2;
    }

    vector<int> cycle_search()
    {
        int n = tree.size();
        vector<int> visited(n, 0), ans;
        for (int i = 0; i < n; ++i) {
            if (visited[i] == 0) {
                bool flag = false;
                int coincidence = -1;
                dfs(visited, ans, i, flag, coincidence);
                if (coincidence != -1) {
                    return ans;
                }
            }
        }
        return vector<int>();
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
    }
    vector<int> ans = g.cycle_search();
    if (ans.size() == 0) {
        cout << "NO" << endl;
    } else {
        cout << "YES" << endl;
        for (auto it = ans.rbegin(); it != ans.rend(); ++it) {
            cout << *it << " ";
        }
        cout << endl;
    }
    return 0;
}

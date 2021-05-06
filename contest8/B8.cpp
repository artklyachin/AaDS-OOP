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
    vector<int> tin, tout;
    int root;

public:
    graph(int N, vector<int>& g)
    {
        for (int i = 0; i < N; ++i) {
            tree.resize(N);
            if (g[i] != 0) {
                tree[g[i] - 1].insert(i);
            } else {
                root = i;
            }
        }
        passage();
    }

    ~graph()
    {
    }


    void dfs(int n, int& time, int v)
    {
        for (int i : tree[v]) {
            tin[i] = time;
            ++time;
            dfs(n, time, i);
            tout[i] = time;
            ++time;
        }
    }

    void passage()
    {
        int N = tree.size();
        tin.resize(N, -1); 
        tout.resize(N, -1);
        int time = 0;
        tin[root] = time;
        ++time;
        dfs(N, time, root);
        tout[root] = time;
        ++time;  
    }

    bool is_ancestor(int u, int v)
    {
        return tin[u - 1] < tin[v - 1] && tout[u - 1] > tout[v - 1];
    }

};



int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, root;
    cin >> n;
    vector<int> tree(n);
    for (int i = 0; i < n; ++i) {
        cin >> tree[i];
    }
    graph g(n, tree);
    int m;
    cin >> m;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        cout << g.is_ancestor(u, v) << endl;
    }
    return 0;
}

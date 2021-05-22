#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <queue>


using namespace std;


struct IGraph {

    virtual void connect(int64_t i, int64_t j, int64_t w) = 0;
    virtual ~IGraph() = default;

};


class graph : public IGraph
{
private:
    vector<vector<pair<int, int>>> g;

public:
    graph(int64_t N)
    {
        g.resize(N, vector<pair<int, int>>());
    }

    ~graph()
    {
    }

    void connect(int64_t i, int64_t j, int64_t w)
    {
        g[i].push_back({ j, w });
    }

    int64_t prim(int64_t N)
    {
        vector<int> visited(N, 0);
        priority_queue<tuple<int, int, int>> q;
        visited[0] = 1;
        for (auto p : g[0]) {
            q.push({ -p.second, 0, p.first });
        }
        int ans = 0;
        while (!q.empty()) {
            auto p = q.top();
            q.pop();
            int c = -get<0>(p), u = get<1>(p), v = get<2>(p);
            if (!visited[v]) {
                ans += c;
                visited[v] = 1;
                for (auto i : g[v]) {
                    q.push({ -i.second, u, i.first });
                }
            }
        }
        return ans;
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;
    graph G(N);
    for (int i = 0; i < M; ++i) {
        int u, v, c;
        cin >> u >> v >> c;
        --u;
        --v;
        G.connect(u, v, c);
        G.connect(v, u, c);
    }
    
    cout << G.prim(N) << endl;
    return 0;
}

/*

4 3
1 2 1
2 3 2
3 1 3
3

*/

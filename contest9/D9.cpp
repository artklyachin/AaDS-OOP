#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>


using namespace std;

const int64_t INF = 2e18 + 1;


struct IGraph {

    virtual void connect(int64_t i, int64_t j, int64_t w) = 0;
    virtual ~IGraph() = default;

};


class graph : public IGraph
{
private:
    vector<tuple<int64_t, int64_t, int64_t>> g;
    vector<unordered_map<int64_t, int64_t>> sp;

public:
    graph(int64_t N)
    { 
        sp.resize(N);
    }

    ~graph()
    { }

    void connect(int64_t i, int64_t j, int64_t w)
    {
        g.push_back(make_tuple(i, j, w));
        sp[i][j] = w;
    }

    vector<int64_t> ford(int64_t N, int64_t s)
    {
        vector<int64_t> d(N, INF), d2(N, INF);
        d[s] = 0;
        for (int64_t q = 0; q < N; ++q) {
            for (int64_t i = 0; i < g.size(); ++i) {
                int64_t from = get<0>(g[i]), to = get<1>(g[i]), weight = get<2>(g[i]);
                if (d[from] == INF) continue;
                d[to] = min(d[to], d[from] + weight);
            }
            if (q == N - 2) {
                d2 = d;
            }
        }
        vector<int64_t> ans = d2;

        for (int64_t i = 0; i < N; ++i) {
            if (d2[i] != d[i] && ans[i] != INF + 1) {
                dfs(ans, i);
            }
        }
        return ans;
    }

    void dfs(vector<int64_t>& ans, int64_t v)
    {
        ans[v] = INF + 1;
        for (auto i : sp[v]) {
            if (ans[i.first] != INF + 1) {
                dfs(ans, i.first);
            }
        }
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t N, M, s;
    cin >> N >> M >> s;
    graph g(N);
    --s;
    for (int64_t i = 0; i < M; ++i) {
        int64_t u, v, w;
        cin >> u >> v >> w;
        --u;
        --v;
        g.connect(u, v, w);
    }
    vector<int64_t> ans = g.ford(N, s);
    for (int64_t i = 0; i < N; ++i) {
        if (ans[i] == INF) {
            cout << "*" << endl;
        } else if (ans[i] == INF + 1) {
            cout << "-" << endl;
        } else {
            cout << ans[i] << endl;
        }
    }
    return 0;
}

/*
1 1 1
1 1 -1000000000000000000

*/
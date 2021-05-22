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
    vector<tuple<int64_t, int64_t, int64_t>> g;

public:
    graph()
    {
    }

    ~graph()
    {
    }

    void connect(int64_t i, int64_t j, int64_t w)
    {
        g.push_back({ i, j, w });
    }

    
    int64_t lider(vector<int64_t>& cnm, int64_t u)
    {
        if (cnm[u] == u) return u;
        cnm[u] = lider(cnm, cnm[u]);
        return cnm[u];
    }


    int64_t kruskal(int64_t N, int64_t M)
    {
        sort(g.begin(), g.end());
        vector<int64_t> cnm(N, 0);
        for (int64_t i = 0; i < N; ++i) {
            cnm[i] = i;
        }
        int64_t ans = 0;
        for (int64_t j = 0; j < M; ++j) {
            int64_t c = get<0>(g[j]), u = get<1>(g[j]), v = get<2>(g[j]);
            if (lider(cnm, u) != lider(cnm, v)) {
                cnm[lider(cnm, u)] = lider(cnm, v);
                ans += c;
            }
        }
        return ans;
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t N, M;
    cin >> N >> M;
    graph G;
    for (int64_t i = 0; i < M; ++i) {
        int64_t u, v, c;
        cin >> u >> v >> c;
        --u;
        --v;
        G.connect(c, u, v);
    }
    cout << G.kruskal(N, M) << endl;
    return 0;
}

/*

4 3
1 2 1
2 3 2
3 1 3
3

3 3
1 2 3
2 3 3
1 3 3

*/

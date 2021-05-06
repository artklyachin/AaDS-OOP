#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;

const int64_t INF = 1e9;


struct IGraph {

    virtual void connect(int64_t i, int64_t j, int64_t w) = 0;
    virtual ~IGraph() = default;

};


class graph : public IGraph
{
private:
    vector<unordered_map<int64_t, int64_t>> g;

public:
    graph(int64_t n)
    {
        g.resize(n);
    }

    ~graph()
    {
    }

    void connect(int64_t i, int64_t j, int64_t w)
    {
        g[i][j] = w;
    }

    int64_t dijkstra(int64_t s, int64_t t)
    {
        int64_t N = g.size();
        priority_queue<pair<int64_t, int64_t>> m;
        vector<int64_t> visited(N, 0), distance(N, INF);
        m.push({ 0, s });
        distance[s] = 0;
        while (true) {
            while (!m.empty() && visited[m.top().second]) {
                m.pop();
            }
            if (m.empty()) break;
            int64_t v = m.top().second;
            visited[v] = 1;
            m.pop();
            for (auto elem : g[v]) {
                int64_t to = elem.first;
                if (visited[to]) continue;
                if (distance[to] > distance[v] + elem.second) {
                    distance[to] = distance[v] + elem.second;
                    m.push({ -distance[to] , to });
                }
            }
        }
        if (distance[t] == INF) return -1;
        return distance[t];
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t N, M, s, t;
    cin >> N >> M;
    cin >> s >> t;
    --s;
    --t;
    graph g(N);
    for (int64_t i = 0; i < M; ++i) {
        int64_t from, to, w;
        cin >> from >> to >> w;
        --from;
        --to;
        g.connect(from, to, w);
        g.connect(to, from, w);
    }
    cout << g.dijkstra(s, t) << endl;
    return 0;
}


/*
4 4
1 3
1 2 1
2 3 2
3 4 5
4 1 4
*/
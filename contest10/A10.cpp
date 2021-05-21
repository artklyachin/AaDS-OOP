#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <queue>


using namespace std;


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;
    vector<vector<pair<int, int>>> g(N, vector<pair<int, int>> ());
    for (int i = 0; i < M; ++i) {
        int u, v, c;
        cin >> u >> v >> c;
        --u;
        --v;
        g[u].push_back({ v, c });
        g[v].push_back({ u, c });
    }
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
    cout << ans << endl;
    return 0;
}

/*

4 3
1 2 1
2 3 2
3 1 3
3

*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <queue>


using namespace std;


int lider(vector<int>& cnm, int u)
{
    if (cnm[u] == u) return u;
    cnm[u] = lider(cnm, cnm[u]);
    return cnm[u];
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;
    vector<tuple<int, int, int>> g;
    for (int i = 0; i < M; ++i) {
        int u, v, c;
        cin >> u >> v >> c;
        --u;
        --v;
        g.push_back({ c, u, v });
    }
    sort(g.begin(), g.end());
    vector<int> cnm(N, 0);
    for (int i = 0; i < N; ++i) {
        cnm[i] = i;
    }
    int ans = 0;
    for (int j = 0; j < M; ++j) {
        int c = get<0>(g[j]), u = get<1>(g[j]), v = get<2>(g[j]);
        if (lider(cnm, u) != lider(cnm, v)) {
            cnm[lider(cnm, u)] = lider(cnm, v);
            ans += c;
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

3 3
1 2 3
2 3 3
1 3 3

*/

#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std;

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    cin >> N >> M;
    vector<pair<int, int>> E(M);
    for (auto& e : E) {
        cin >> e.first >> e.second;
        --e.first;
        --e.second;
        if (e.first > e.second) {
            swap(e.first, e.second);
        }
    }
    vector<unordered_set<int>> V(N);
    for (auto& e : E) {
        V[e.first].insert(e.second);
    }
    int cycles = 0;
    for (int u = 0; u < N; ++u) {
        for (int v : V[u]) {
            for (int w : V[v]) {
                if (V[u].count(w) != 0) {
                    ++cycles;
                }
            }
        }
    }
    cout << cycles / 4 << "." << cycles % 4 * 25 << "\n";
    return 0;
}
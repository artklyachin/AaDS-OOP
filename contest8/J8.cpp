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
    vector<unordered_map<int, int>> g;
    vector<pair<int, int>> ribs;
    vector<int> cicle;
    int firstrib;
    int n;


    int count, firstv;

public:
    graph(int N, int A)
        : n(N)
    {
        g.resize(n);
        count = 0;
        firstv = A - 1;
    }

    ~graph()
    {
    }


    void connect(int from, int to, int connection)
    {
        if (!connection && from != to) {
            g[from][to] = count;
            ++count;
            ribs.push_back({ from, to });
        }
    }

    int dfs(vector<pair<int, int>>& loop_ends, unordered_set<int>& tops, int rib, int v)
    {
        tops.insert(v);
        if (g[v].size() == 0) {
            return rib;
        }
        auto k = *g[v].begin();
        cicle[rib] = k.second;
        if (loop_ends[v].first == -1) {
            loop_ends[v] = { rib, k.second };
        }
        g[v].erase(k.first);
        dfs(loop_ends, tops, k.second, k.first);
    }

    void build_hyperchannels()
    {
        firstrib = g[firstv].begin()->second;
        cicle.resize(count, -1);
        vector<pair<int, int>> loop_ends(n, { -1, -1 });
        unordered_set<int> tops;
        tops.insert(firstv);
        int i = *tops.begin();
        bool flag = true;
        while (tops.size() > 0) {
            auto k = *g[i].begin();
            g[i].erase(k.first);
            int rib = dfs(loop_ends, tops, k.second, k.first);
            if (flag) {
                cicle[rib] = k.second;
                loop_ends[i] = { rib, k.second };
                flag = false;
            } else {
                cicle[rib] = loop_ends[i].second;
                cicle[loop_ends[i].first] = k.second;
                loop_ends[i].second = k.first;
            }
            while (g[i].size() == 0) {
                tops.erase(i);
                if (tops.size() == 0) break;
                i = *tops.begin();
            }
        }
    }


    vector<pair<int, int>> output_of_necessary_hyperchannels()
    {
        vector<pair<int, int>> ans;
        if (count == 0) {
            return ans;
        }
        build_hyperchannels();
        int rib = cicle[firstrib];
        while (rib != firstrib) {
            ans.push_back({ ribs[rib].first + 1, ribs[rib].second + 1 });
            rib = cicle[rib];
        }
        return ans;
    }

};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, a;
    cin >> n >> a;
    graph g(n, a);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int connection;
            cin >> connection;
            g.connect(i, j, connection);
        }
    }
    vector<pair<int, int>> ans = g.output_of_necessary_hyperchannels();
    for (auto i : ans) {
        cout << i.first << " " << i.second << endl;
    }
    return 0;
}

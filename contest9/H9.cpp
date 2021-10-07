#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std;


inline int get(uint64_t v, int i)
{
    return (v >> (4 * i)) & 15;
}

inline void set(uint64_t& v, int i, int digit)
{
    v &= ~(uint64_t(15) << (4 * i));
    v |= (uint64_t(digit) << (4 * i));
}

struct IGraph {

    virtual ~IGraph() = default;

};


class graph : public IGraph
{
private:
    vector<unordered_set<uint64_t>> vum;
    vector<int> ar;
    vector<int> b;

public:
    graph(int n, int d)
    {
        ar.resize(n);
        for (int i = 0; i < n; ++i) {
            int x;
            cin >> x;
            ar[x - 1] = i;
        }
        b.resize(n);
        for (int i = 0; i < n; ++i) {
            int x;
            cin >> x;
            b[i] = ar[x - 1];
        }
        uint64_t a = 0;
        for (int i = 0; i < n; ++i) {
            set(a, i, i);
        }
        vum.resize(d + 1);
        vum[0].insert(a);
        BFS(n, d);
    }

    ~graph()
    {
    }

    void BFS(int n, int d)
    {
        for (int i = 1; i <= d; ++i) {
            for (auto w : vum[i - 1]) {
                for (int l = 0; l < n - 1; ++l) {
                    for (int r = l + 1; r < n; ++r) {
                        uint64_t v = w;
                        for (int li = l, ri = r; li < ri; ++li, --ri) {
                            int v1 = get(v, li);
                            int v2 = get(v, ri);
                            set(v, li, v2);
                            set(v, ri, v1);
                        }
                        vum[i].insert(v);
                    }
                }
            }
        }
    }

    int find_distance()
    {
        int n = ar.size(), answer = n - 1;
        for (int i = 0; i < int(vum.size()) && i + i < answer; ++i) {
            for (auto& p : vum[i]) {
                uint64_t t = 0;
                for (int j = 0; j < n; ++j) {
                    set(t, j, b[get(p, j)]);
                }
                for (int j = i; j < int(vum.size()) && i + j < answer; ++j) {
                    if (vum[j].count(t)) {
                        answer = i + j;
                    }
                }
            }
        }
        return answer;
    }
};



int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    cin >> n;
    graph g(n, (n - 1) / 2);
    cout << g.find_distance() << endl;
    return 0;
}
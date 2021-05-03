#include <iostream>
#include <unordered_set>
#include <algorithm>

using namespace std;

int W, H;

struct MyHash
{
    std::size_t operator()(pair<int, int> const& p) const noexcept
    {
        return size_t(p.second) * (H + 2) + p.first;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N;
    cin >> W >> H >> N;
    bool flag = true;
    unordered_set<pair<int, int>, MyHash> S;
    for (int i = 0; i < N; ++i) {
        int x, y;
        cin >> x >> y;
        if (x >= 0 && x <= W + 1 && y >= 0 && y <= H + 1) {
            S.emplace(x, y);
        }
    }
    if (5 * N < W * H) {
        flag = false;
    }
    for (int x = 1; flag && x <= W; ++x) {
        for (int y = 1; flag && y <= H; ++y) {
            if (S.find({ x, y }) == S.end() && S.find({ x - 1, y }) == S.end() && S.find({ x + 1, y }) == S.end() && S.find({ x, y - 1 }) == S.end() && S.find({ x, y + 1 }) == S.end()) {
                flag = false;
                break;
            }
        }
    }
    cout << (flag ? "Yes\n" : "No\n");
    return 0;
}
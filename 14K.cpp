#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("fast-math")
#pragma GCC optimize("profile-values,profile-reorder-functions,tracer")
#pragma GCC optimize("vpt")
#pragma GCC optimize("rename-registers")
#pragma GCC optimize("move-loop-invariants")
#pragma GCC optimize("unswitch-loops")
#pragma GCC optimize("function-sections")
#pragma GCC optimize("data-sections")
#pragma GCC optimize("branch-target-load-optimize")
#pragma GCC optimize("branch-target-load-optimize2")
#pragma GCC optimize("btr-bb-exclusive")
#pragma GCC optimize("Ofast,no-stack-protector")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,tune=native")

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>


struct Point
{
    int x, y;
};

Point V[3072];

int ij, ik, jk, best;

inline int sqr(int x)
{
    return x * x;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    std::cin >> N;
    for (int i = 0; i < N; ++i) {
        std::cin >> V[i].x >> V[i].y;
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < i; ++j) {
            for (int k = 0; k < j; ++k) {
                ij = (V[i].x - V[j].x) * (V[i].x - V[j].x) + (V[i].y - V[j].y) * (V[i].y - V[j].y);
                ik = (V[i].x - V[k].x) * (V[i].x - V[k].x) + (V[i].y - V[k].y) * (V[i].y - V[k].y);
                jk = (V[j].x - V[k].x) * (V[j].x - V[k].x) + (V[j].y - V[k].y) * (V[j].y - V[k].y);
                best = std::max(best, std::min(ij, std::min(ik, jk)));
            }
        }
    }
    std::cout << std::setprecision(15) << sqrt(double(best)) / 2 << "\n";
    return 0;
}
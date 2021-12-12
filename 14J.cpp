#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>


double INF = 1e9;

double right_border_of_lightning(double left_border, double x, double y, double alpha)
{
    double betta = atan2(x - left_border, y);
    if (alpha - betta >= M_PI / 2) {
        return INF;
    }
    return x + y * tan(alpha - betta);

}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    double L, R;
    std::cin >> N >> L >> R;
    std::vector<double> X(N), Y(N), Alpha(N);
    for (int i = 0; i < N; ++i) {
        double degrees;
        std::cin >> X[i] >> Y[i] >> degrees;
        Alpha[i] = (degrees * M_PI / 180);
    }
    std::vector<double> results(1 << N, L);
    for (int mask = 0; mask < (1 << N); ++mask) {
        for (int i = 0; i < N; ++i) {
            if (!(mask & (1 << i))) continue;
            results[mask] = std::max(results[mask], right_border_of_lightning(results[mask ^ (1 << i)], X[i], Y[i], Alpha[i]));
            if (results[mask] >= R) {
                std::cout << std::setprecision(10);
                std::cout << R - L << "\n";
                return 0;
            }
        }
    }
    std::cout << std::setprecision(10);
    std::cout << results[(1 << N) - 1] - L << "\n";
    return 0;
}
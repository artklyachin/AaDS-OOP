#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <cstdint>
#include <tuple>


using I = int64_t; // тип для представления координат
using II = __int128; // тип для вычисление произведений чисел типа I

template<class T>
int sign(const T& v)
{
    if (v < 0) {
        return -1;
    } else if (v > 0) {
        return 1;
    } else {
        return 0;
    }
}

struct Point
{
    I x, y;

    Point()
        : x(0), y(0)
    {
    }

    Point(I _x, I _y)
        : x(_x), y(_y)
    {
    }

    Point(const Point& rhs)
        : x(rhs.x), y(rhs.y)
    {
    }

    Point& operator=(const Point& rhs)
    {
        if (this != &rhs) {
            x = rhs.x;
            y = rhs.y;
        }
        return *this;
    }

    bool operator==(const Point& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Point& rhs) const
    {
        return !operator==(rhs);
    }

    bool operator<(const Point& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    Point operator+(const Point& rhs) const
    {
        return Point(x + rhs.x, y + rhs.y);
    }

    Point& operator+=(const Point& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Point operator-()
    {
        return Point(-x, -y);
    }

    Point operator-(const Point& rhs) const
    {
        return Point(x - rhs.x, y - rhs.y);
    }

    Point& operator-=(const Point& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Point operator*(I k) const
    {
        return Point(x * k, y * k);
    }

    Point& operator*=(I k)
    {
        x *= k;
        y *= k;
        return *this;
    }

    II dot(const Point& rhs) const
    {
        return II(x) * rhs.x + II(y) * rhs.y;
    }

    II prod(const Point& rhs) const
    {
        return II(x) * rhs.y - II(y) * rhs.x;
    }
};

bool isOrdered(const I& a, const I& b, const I& c)
{
    return II(c - b) * (b - a) >= 0;
}

bool isOrdered(const Point& A, const Point& B, const Point& C)
{
    return isOrdered(A.x, B.x, C.x) && isOrdered(A.y, B.y, C.y);
}

int rotationDirection(const Point& A, const Point& B, const Point& C)
{
    return sign((B - A).prod(C - A));
}

bool counterClockWise(const Point& A, const Point& B, const Point& C)
{
    return (B - A).prod(C - A) > 0; // A.prod(B) + B.prod(C) + C.prod(A) > 0;
}

bool clockWise(const Point& A, const Point& B, const Point& C)
{
    return (B - A).prod(C - A) < 0; // A.prod(B) + B.prod(C) + C.prod(A) < 0;
}

std::vector<Point> readAndPrepare()
{
    int N;
    std::cin >> N;
    std::vector<Point> V(N == 1 ? 1 : N + 1);
    for (int i = 0; i < N; ++i) {
        std::cin >> V[i].x >> V[i].y;
    }
    rotate(V.begin(), min_element(V.begin(), V.begin() + N), V.begin() + N);
    V.back() = V[0];
    return V;
}


int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto V1 = readAndPrepare();
    auto V2 = readAndPrepare();
    auto V3 = readAndPrepare();
    std::vector<Point> D;
    D.reserve(V1.size() + V2.size() + V3.size());
    D.emplace_back(V1[0] + V2[0] + V3[0]);
    auto it1 = V1.empty() ? V1.end() : V1.begin() + 1;
    auto it2 = V2.empty() ? V2.end() : V2.begin() + 1;
    auto it3 = V3.empty() ? V3.end() : V3.begin() + 1;
    while (it1 != V1.end() && it2 != V2.end() && it3 != V3.end()) {
        Point A1 = *it1 - *prev(it1);
        Point A2 = *it2 - *prev(it2);
        Point A3 = *it3 - *prev(it3);
        if (A1.prod(A2) >= 0 && A1.prod(A3) >= 0) {
            D.emplace_back(D.back() + A1);
            ++it1;
        }
        if (A2.prod(A1) >= 0 && A2.prod(A3) >= 0) {
            D.emplace_back(D.back() + A2);
            ++it2;
        }
        if (A3.prod(A1) >= 0 && A3.prod(A2) >= 0) {
            D.emplace_back(D.back() + A3);
            ++it3;
        }
    }
    while (it1 != V1.end() && it2 != V2.end()) {
        Point A1 = *it1 - *prev(it1);
        Point A2 = *it2 - *prev(it2);
        if (A1.prod(A2) >= 0) {
            D.emplace_back(D.back() + A1);
            ++it1;
        }
        if (A2.prod(A1) >= 0) {
            D.emplace_back(D.back() + A2);
            ++it2;
        }
    }
    while (it1 != V1.end() && it3 != V3.end()) {
        Point A1 = *it1 - *prev(it1);
        Point A3 = *it3 - *prev(it3);
        if (A1.prod(A3) >= 0) {
            D.emplace_back(D.back() + A1);
            ++it1;
        }
        if (A3.prod(A1) >= 0) {
            D.emplace_back(D.back() + A3);
            ++it3;
        }
    }
    while (it2 != V2.end() && it3 != V3.end()) {
        Point A2 = *it2 - *prev(it2);
        Point A3 = *it3 - *prev(it3);
        if (A2.prod(A3) >= 0) {
            D.emplace_back(D.back() + A2);
            ++it2;
        }
        if (A3.prod(A2) >= 0) {
            D.emplace_back(D.back() + A3);
            ++it3;
        }
    }
    while (it1 != V1.end()) {
        Point A1 = *it1 - *prev(it1);
        D.emplace_back(D.back() + A1);
        ++it1;
    }
    while (it2 != V2.end()) {
        Point A2 = *it2 - *prev(it2);
        D.emplace_back(D.back() + A2);
        ++it2;
    }
    while (it3 != V3.end()) {
        Point A3 = *it3 - *prev(it3);
        D.emplace_back(D.back() + A3);
        ++it3;
    }
    const auto& Mn = D[0];
    auto itMx = max_element(D.begin(), D.end());
    auto ritMx = max_element(D.rbegin(), D.rend());
    const auto& Mx = *itMx;
    I yMin = D[0].y, yMax = D[0].y;
    for (int i = 1; i < int(D.size()); ++i) {
        yMin = std::min(yMin, D[i].y);
        yMax = std::max(yMax, D[i].y);
    }
    int Q;
    std::cin >> Q;
    for (int q = 0; q < Q; ++q) {
        Point X;
        std::cin >> X.x >> X.y;
        Point A = X * 3;
        if (A < Mn || Mx < A || A.y < yMin || yMax < A.y) {
            std::cout << "NO\n";
            continue;
        }
        int d = rotationDirection(A, Mn, Mx);
        if (d == 0) {
            std::cout << "YES\n";
        } else if (d < 0) {
            auto it = lower_bound(D.begin(), itMx, A);
            Point B = *prev(it);
            Point C = *it;
            std::cout << (rotationDirection(A, B, C) >= 0 ? "YES\n" : "NO\n");
        } else {
            auto it = lower_bound(D.rbegin(), ritMx, A);
            Point B = *prev(it);
            Point C = *it;
            std::cout << (rotationDirection(A, B, C) <= 0 ? "YES\n" : "NO\n");
        }
    }
    return 0;
}
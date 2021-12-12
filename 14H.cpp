#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <set>


using I = int64_t; // тип для представления координат
using II = int64_t; // тип для вычисление произведений чисел типа T

II square(I k)
{
    return k * k;
}

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

    bool operator<(const Point& rhs) const
    {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }
};

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

class ConvexHull
{
    using S = std::set<Point>;
    using SI = std::set<Point>::iterator;
    std::set<Point> up, down;

public:
    ConvexHull()
    {
    }

    void up_point(Point O)
    {
        SI it;
        bool ok;
        tie(it, ok) = up.insert(O);
        if (!ok) return;
        if (it != up.begin() && next(it) != up.end() && !clockWise(*prev(it), *it, *next(it))) {
            up.erase(it);
            return;
        }

        if (next(it) != up.end()) {
            while (next(next(it)) != up.end() && !clockWise(*it, *next(it), *next(next(it)))) {
                up.erase(next(it));
            }
        }

        if (it != up.begin()) {
            while (prev(it) != up.begin() && !counterClockWise(*it, *prev(it), *prev(prev(it)))) {
                up.erase(prev(it));
            }
        }
    }

    void down_point(Point O)
    {
        SI it;
        bool ok;
        tie(it, ok) = down.insert(O);
        if (!ok) return;
        if (it != down.begin() && next(it) != down.end() && !counterClockWise(*prev(it), *it, *next(it))) {
            down.erase(it);
            return;
        }

        if (next(it) != down.end()) {
            while (next(next(it)) != down.end() && !counterClockWise(*it, *next(it), *next(next(it)))) {
                down.erase(next(it));
            }
        }

        if (it != down.begin()) {
            while (prev(it) != down.begin() && !clockWise(*it, *prev(it), *prev(prev(it)))) {
                down.erase(prev(it));
            }
        }
    }

    void add(Point O)
    {
        up_point(O);
        down_point(O);
    }

    bool inConvexHull(Point O)
    {
        auto MN = *up.begin(), MX = *prev(up.end());
        if (O < MN || MX < O) {
            return false;
        }

        int d = rotationDirection(MN, MX, O);
        if (d == 0) {
            return true;
        }

        if (d > 0) { // up
            auto up_it = up.lower_bound(O);
            if (O == *up_it) {
                return true;
            }
            return rotationDirection(*prev(up_it), *up_it, O) <= 0;
        } else {
            auto down_it = down.lower_bound(O);
            if (O == *down_it) {
                return true;
            }
            return rotationDirection(*prev(down_it), *down_it, O) >= 0;
        }
    }
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    std::cin >> N;
    ConvexHull hull;
    for (int i = 0; i < N; ++i) {
        int t, x, y;
        std::cin >> t >> x >> y;
        if (t == 1) {
            hull.add(Point(x, y));
        } else {
            std::cout << (hull.inConvexHull(Point(x, y)) ? "YES\n" : "NO\n");
        }
    }
    return 0;
}
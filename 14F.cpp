#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <tuple>

using I = int; // тип для представления координат
using II = int64_t; // тип для вычисление произведений чисел типа I

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
    {}

    Point(I _x, I _y)
        : x(_x), y(_y)
    {}

    Point(const Point& rhs) = default;

    Point& operator=(const Point& rhs) = default;

    bool operator==(const Point& rhs) const
    {
        return std::tie(x, y) == std::tie(rhs.x, rhs.y);
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

    Point operator-() const
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

int rotationDirection(const Point& A, const Point& B, const Point& C)
{
    return sign((B - A).prod(C - A));
}

int counterClockWise(const Point& A, const Point& B, const Point& C)
{
    return (B - A).prod(C - A) > 0;
}

int clockWise(const Point& A, const Point& B, const Point& C)
{
    return (B - A).prod(C - A) < 0;
}

std::vector<Point> buildConvexHull(std::vector<Point> V)
{
    if (V.size() <= 1) {
        return V;
    }
    std::sort(V.begin(), V.end());
    std::vector<Point> up, down;
    up.emplace_back(V[0]);
    down.emplace_back(V[0]);
    for (size_t i = 1; i < V.size(); ++i) {
        int direction = rotationDirection(V[0], V[i], V.back());
        if (i == V.size() - 1 || direction < 0) {
            while (up.size() >= 2 && !clockWise(up[up.size() - 2], up.back(), V[i])) {
                up.pop_back();
            }
            up.emplace_back(V[i]);
        }
        if (i == V.size() - 1 || direction > 0) {
            while (down.size() >= 2 && !counterClockWise(down[down.size() - 2], down.back(), V[i])) {
                down.pop_back();
            }
            down.emplace_back(V[i]);
        }
    }
    int downSz = down.size();
    int upSz = up.size();
    std::vector hull(move(down));
    hull.resize(downSz + upSz - 2);
    copy(up.rbegin() + 1, up.rbegin() + upSz - 1, hull.begin() + downSz);
    return hull;
}

std::vector<Point> readInput(std::istream& is = std::cin)
{
    int N;
    is >> N;
    std::vector<Point> points(N);
    for (Point& point : points) {
        is >> point.x >> point.y;
    }
    return points;
}

void outputResult(const std::vector<Point>& hull, std::ostream& os = std::cout)
{
    os << hull.size() << std::endl;
    for (const auto& point: hull) {
        os << point.x << " " << point.y << "\n";
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Point> points = readInput();
    std::vector<Point> hull = buildConvexHull(points);
    outputResult(hull);
    return 0;
}

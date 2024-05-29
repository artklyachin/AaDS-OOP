#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <tuple>


using I = int; // тип для представления координат
using II = int64_t; // тип для вычисление произведений чисел типа I

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

    void reflect(const Point& center)
    {
        x = -x;
        y = -y;
        *this += center * 2;
    }

    void scale(const Point& center, I coefficient)
    {
        *this -= center;
        *this *= coefficient;
        *this += center;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "(" << x << "," << y << ")";
        return ss.str();
    }

    bool isOrigin() const
    {
        return x == 0 && y == 0;
    }

    int quadrant() const
    {
        if (x == 0 && y == 0) {
            return 0;
        } else if (x > 0 && y >= 0) {
            return 1;
        } else if (x <= 0 && y > 0) {
            return 2;
        } else if (x < 0 && y <= 0) {
            return 3;
        } else {
            return 4;
        }
    }
};

int rotationDirection(const Point& A, const Point& B, const Point& C)
{
    return sign((B - A).prod(C - A));
}

int counterClockWise(const Point& A, const Point& B, const Point& C)
{
    return (B - A).prod(C - A) > 0; // A.prod(B) + B.prod(C) + C.prod(A) > 0;
}

int clockWise(const Point& A, const Point& B, const Point& C)
{
    return (B - A).prod(C - A) < 0; // A.prod(B) + B.prod(C) + C.prod(A) < 0;
}

struct OrderByXY
{
    bool operator()(const Point& A, const Point& B)
    {
        return std::tie(A.x, A.y) < std::tie(B.x, B.y);
    }
};

struct OrderByArg
{
    // Arg is belong to [0, 2*PI)
    bool operator()(const Point& A, const Point& B)
    {
        int rc = A.quadrant() - B.quadrant();
        if (rc != 0) {
            return rc < 0;
        }
        return A.prod(B) > 0;
    }
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    std::cin >> N;
    std::vector<Point> V(N);
    for (Point& A : V) {
        std::cin >> A.x >> A.y;
    }
    int positive = 0, negative = 0;
    for (int i = 0; i < N; ++i) {
        int direction = rotationDirection(V[i], V[(i + 1) % N], V[(i + 2) % N]);
        if (direction > 0) {
            ++positive;
        } else if (direction < 0) {
            ++negative;
        }
    }
    if ((positive > 0 && negative > 0) || (positive == 0 && negative == 0)) {
        std::cout << "NO\n";
    } else {
        std::cout << "YES\n";
    }
    return 0;
}
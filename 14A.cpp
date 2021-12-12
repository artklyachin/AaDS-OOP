﻿#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <iomanip>

const double ABS_EPS = 1e-9;
const double PI = acos(-1.0);
const double RAD_IN_DEG = PI / 180.0;

bool isZero(double v)
{
    return fabs(v) < ABS_EPS;
}

bool isEqual(double lhs, double rhs)
{
    double d = fabs(rhs - lhs);
    return d < ABS_EPS;
}

double square(double k)
{
    return k * k;
}

struct Point
{
    double x, y;

    Point()
        : x(0.), y(0.)
    {
    }

    Point(double _x, double _y)
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
        return isEqual(x, rhs.x) && isEqual(y, rhs.y);
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

    Point operator*(double k) const
    {
        return Point(x * k, y * k);
    }

    Point& operator*=(double k)
    {
        x *= k;
        y *= k;
        return *this;
    }

    Point operator/(double k) const
    {
        return Point(x / k, y / k);
    }

    Point& operator/=(double k)
    {
        x /= k;
        y /= k;
        return *this;
    }

    double dot(const Point& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    double prod(const Point& rhs) const
    {
        return x * rhs.y - y * rhs.x;
    }

    double distTo(const Point& rhs) const
    {
        double dx = rhs.x - x, dy = rhs.y - y;
        return sqrt(dx * dx + dy * dy);
    }

    void rotate(double angleInDegrees)
    {
        double c = cos(angleInDegrees * RAD_IN_DEG);
        double s = sin(angleInDegrees * RAD_IN_DEG);
        double x1 = x * c - y * s;
        double y1 = x * s + y * c;
        x = x1;
        y = y1;
    }

    void rotate(const Point& center, double angleInDegrees)
    {
        *this -= center;
        rotate(angleInDegrees);
        *this += center;
    }

    void reflect(const Point& center)
    {
        x = -x;
        y = -y;
        *this += center * 2;
    }

    void scale(const Point& center, double coefficient)
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

    double abs() const
    {
        return sqrt(x * x + y * y);
    }

    double arg() const
    {
        return (x != 0 || y != 0) ? atan2(y, x) : 0;
    }
};

class Line
{
    double a, b, c; // a*x + b*y + c == 0

public:
    Line()
        : a(0.), b(1.), c(0.)
    {
    }

    Line(double _a, double _b, double _c)
        : a(_a), b(_b), c(_c)
    {
    }

    Line(double px, double py, double qx, double qy)
        : Line(Point(px, py), Point(qx, qy))
    {
    }

    Line(const Line& rhs)
        : a(rhs.a), b(rhs.b), c(rhs.c)
    {
    }

    Line(const Point& p, const Point& q)
        : a(p.y - q.y), b(q.x - p.x), c(p.prod(q))
    {
    }

    Line(const Point& p, double k)
        : a(k), b(-1), c(p.y - k * p.x)
    {
    }

    Line(double k, double s) // y == k*x + s
        : a(k), b(-1.), c(s)
    {
    }

    Line& operator=(const Line& rhs)
    {
        if (this != &rhs) {
            a = rhs.a;
            b = rhs.b;
            c = rhs.c;
        }
        return *this;
    }

    bool operator==(const Line& rhs) const
    {
        return isZero(a * rhs.b - b * rhs.a) && isZero(a * rhs.c - c * rhs.a);
    }

    bool operator!=(const Line& rhs) const
    {
        return !operator==(rhs);
    }

    Point reflectionOfPoint(const Point& p) const
    {
        double alpha = 2.0 * (p.x * a + p.y * b + c) / (a * a + b * b);
        return p - Point(a, b) * alpha;
    }

    Point direction() const
    {
        if (b > 0) {
            return Point(b, -a);
        } else if (b < 0) {
            return Point(-b, a);
        } else {
            return Point(0, abs(a));
        }
    }

    bool isParallelWith(const Line& m) const
    {
        return isZero(a * m.b - b * m.a);
    }

    Point intersectionWith(const Line& m) const
    {
        double d = 1.0 / (a * m.b - b * m.a);
        return Point((b * m.c - c * m.b) * d, (c * m.a - a * m.c) * d);
    }

    double distanceTo(const Line& m) const
    {
        return abs(c - m.c) / sqrt(a * a + b * b);
    }

    Point projectionOfPoint(const Point& p) const
    {
        double alpha = (p.x * a + p.y * b + c) / (a * a + b * b);
        return p - Point(a, b) * alpha;
    }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "[" << a << "* x + " << b << " * y + " << c << " == 0]";
        return ss.str();
    }
};

bool isOrdered(double a, double b, double c)
{
    return (c - b) * (b - a) >= 0;
}

bool isOrdered(const Point& A, const Point& B, const Point& C)
{
    return isOrdered(A.x, B.x, C.x) && isOrdered(A.y, B.y, C.y);
}

double rotationAngle(const Point& O, const Point& A, const Point& B)
{
    double diff = (B - O).arg() - (A - O).arg();
    if (diff <= -PI) {
        return diff + 2 * PI;
    } else if (diff > PI) {
        return diff - 2 * PI;
    } else {
        return diff;
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Point A, B, C, D;
    std::cin >> A.x >> A.y >> B.x >> B.y >> C.x >> C.y >> D.x >> D.y;
    std::cout << std::setprecision(10);
    std::cout << A.distTo(B) << " " << C.distTo(D) << "\n";
    auto E = B - A + D - C;
    std::cout << E.x << " " << E.y << "\n";
    std::cout << (B - A).dot(D - C) << " " << (B - A).prod(D - C) << "\n";
    std::cout << abs((B - A).prod(D - C)) * 0.5 << "\n";
    return 0;
}
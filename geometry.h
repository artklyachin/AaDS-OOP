#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
#include <cmath>


using namespace std;

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
    {}

    Point(double _x, double _y)
        : x(_x), y(_y)
    { }

    Point(const Point& rhs)
        : x(rhs.x), y(rhs.y)
    { }

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

    string toString() const
    {
        stringstream ss;
        ss << "(" << x << "," << y << ")";
        return ss.str();
    }
};

class Line
{
    double a, b, c; 

public:
    Line()
        : a(0.), b(1.), c(0.)
    { }

    Line(double _a, double _b, double _c)
        : a(_a), b(_b), c(_c)
    { }

    Line(double px, double py, double qx, double qy)
        : Line(Point(px, py), Point(qx, qy))
    { }

    Line(const Line& rhs)
        : a(rhs.a), b(rhs.b), c(rhs.c)
    { }

    Line(const Point& p, const Point& q)
        : a(p.y - q.y), b(q.x - p.x), c(p.prod(q))
    { }

    Line(const Point& p, double k)
        : a(k), b(-1), c(p.y - k * p.x)
    { }

    Line(double k, double s) // y == k*x + s
        : a(k), b(-1.), c(s)
    { }

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

    Point intersection(const Line& m) const
    {
        double d = 1.0 / (a * m.b - b * m.a);
        return Point((b * m.c - c * m.b) * d, (c * m.a - a * m.c) * d);
    }

    Point projectionOfPoint(const Point& p) const
    {
        double alpha = (p.x * a + p.y * b + c) / (a * a + b * b);
        return p - Point(a, b) * alpha;
    }

    string toString() const
    {
        stringstream ss;
        ss << "[" << a << "* x + " << b << " * y + " << c << " == 0]";
        return ss.str();
    }
};

class Shape
{
public:
    virtual ~Shape() = default;

    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    bool operator==(const Shape& another) const
    {
        return isEqualTo(another);
    }
    bool operator!=(const Shape& another) const
    {
        return !isEqualTo(another);
    }
    virtual bool isEqualTo(const Shape& another) const = 0;
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point& center, double angle) = 0;
    virtual void reflex(const Point& center) = 0;
    virtual void reflex(const Line& axis) = 0;
    virtual void scale(const Point& center, double coefficient) = 0;

    virtual string className() const = 0;
    virtual string toString() const = 0;
};

class Ellipse : public Shape
{
    Point f1, f2;
    double a;

public:
    Ellipse(const Point& _f1, const Point& _f2, double sumOfDistancesToFoci)
        : f1(_f1), f2(_f2), a(fabs(sumOfDistancesToFoci * 0.5))
    { }

    virtual double perimeter() const override
    {
        return 4 * a * comp_ellint_2(eccentricity());
    }

    virtual double area() const override
    {
        return PI * getA() * getB();
    }

    virtual bool isEqualTo(const Shape & another) const override
    {
        const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&another);
        if (ellipse != nullptr) {
            return ((f1 == ellipse->f1 && f2 == ellipse->f2) || (f1 == ellipse->f2 && f2 == ellipse->f1)) && ::isEqual(a, ellipse->a);
        }
        return false;
    }

    virtual bool isCongruentTo(const Shape & another) const override
    {
        const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&another);
        if (ellipse != nullptr) {
            return a == ellipse->a && eccentricity() == ellipse->eccentricity();
        }
        return false;
    }

    virtual bool isSimilarTo(const Shape & another) const override
    {
        const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&another);
        if (ellipse != nullptr) {
            return eccentricity() == ellipse->eccentricity();
        }
        return false;
    }

    virtual bool containsPoint(const Point & point) const override
    {
        double dist = f1.distTo(point) + f2.distTo(point);
        return dist < 2 * a || isEqual(dist, 2 * a);
    }

    virtual void rotate(const Point & center, double angle) override
    {
        f1.rotate(center, angle);
        f2.rotate(center, angle);
    }

    virtual void reflex(const Point & center) override
    {
        f1.reflect(center);
        f2.reflect(center);
    }

    virtual void reflex(const Line & axis) override
    {
        f1 = axis.reflectionOfPoint(f1);
        f2 = axis.reflectionOfPoint(f2);
    }

    virtual void scale(const Point & center, double coefficient) override
    {
        f1.scale(center, coefficient);
        f2.scale(center, coefficient);
        a *= fabs(coefficient);
    }

    std::pair<Point, Point> focuses() const
    {
        return { f1, f2 };
    }
    
    std::pair<Line, Line> directrices() const
    {
        Point ec = center();
        double s = square(getA() / getC()) * 0.5;
        Point dir = f2 - f1;
        Point ortDir(f2.y - f1.y, - f2.x + f1.x);
        Point d1 = ec + dir * s;
        Point d2 = ec - dir * s;
        return { Line(d1, d1 + ortDir), Line(d2, d2 + ortDir) };
    }

    double eccentricity() const
    {
        return getC() / getA();
    }

    Point center() const
    {
        return Point((f1.x + f2.x) * 0.5, (f1.y + f2.y) * 0.5);
    }

    double getA() const
    {
        return a;
    }

    double getB() const
    {
        double c = getC();
        return sqrt(a * a - c * c);
    }

    double getC() const
    {
        return f1.distTo(f2) * 0.5;
    }

    virtual string className() const override
    {
        return "Ellipse";
    }

    virtual string toString() const override
    {
        stringstream ss;
        ss << className() << "{" << f1.toString() << ", " << f2.toString() << ", a:" << a << ", b:" << getB() << ", c:" << getC() << ", e:" << eccentricity() << "}";
        return ss.str();
    }
};

class Circle : public Ellipse
{
public:
    Circle(const Point& c, double radius)
        : Ellipse(c, c, radius * 2)
    { }

    virtual double perimeter() const override
    {
        return 2 * PI * radius();
    }

    virtual double area() const override
    {
        return PI * radius() * radius();
    }

    double radius() const
    {
        return getA();
    }

    virtual string className() const override
    {
        return "Circle";
    }
};

class Polygon : public Shape
{
protected:
    vector<Point> points;
public:
    Polygon()
    { }

    Polygon(const vector<Point>& _points)
        : points(_points)
    { }

    Polygon(initializer_list<Point> init)
        : points(init)
    { }

    template <class... T>
    Polygon(const Point& first, T... rest)
        : points({ first, rest... })
    { }

    Polygon(const Polygon& rhs)
        : points(rhs.points)
    { }

    Polygon& operator=(const Polygon& rhs)
    {
        if (this != &rhs) {
            points = rhs.points;
        }
        return *this;
    }

    virtual double perimeter() const override
    {
        double p = 0;
        for (int i = 0; i < size(); ++i) {
            p += points[i].distTo(point(i + 1));
        }
        return p;
    }

    virtual double area() const override
    {
        double s = 0;
        for (int i = 0; i < size(); ++i) {
            Point p = point(i);
            Point q = point(i + 1);
            s += (q.x - p.x) * (p.y + q.y) * 0.5;
        }
        return fabs(s);
    }

    virtual bool isEqualTo(const Shape& another) const override
    {
        const Polygon* polygon = dynamic_cast<const Polygon*>(&another);
        if (polygon == nullptr || size() != polygon->size()) {
            return false;
        }
        for (int shift = 0; shift < size(); ++shift) {
            if (point(0) != polygon->point(shift)) {
                continue;
            }
            for (int dir = 1; dir >= -1; dir -= 2) {
                bool flag = true;
                for (int i = 1; i < size() && flag; ++i) {
                    if (point(i) != polygon->point(shift + i * dir)) {
                        flag = false;
                    }
                }
                if (flag) {
                    return true;
                }
            }
            return false;
        }
        return false;
    }

    virtual bool isCongruentTo(const Shape& another) const override
    {
        const Polygon* polygon = dynamic_cast<const Polygon*>(&another);
        if (polygon == nullptr || size() != polygon->size()) {
            return false;
        }
        if (size() < 2) {
            return true;
        }
        for (int shift = 0; shift < size(); ++shift) {
            for (int dir = 1; dir >= -1; dir -= 2) {
                for (int reflection = 1; reflection >= -1; reflection -= 2) {
                    bool flag = true;
                    for (int i = 0; i < size() && flag; ++i) {
                        Point p0 = point(i), p1 = point(i + 1), p2 = point(i + 2);
                        Point q0 = polygon->point(shift + i * dir), q1 = polygon->point(shift + (i + 1) * dir), q2 = polygon->point(shift + (i + 2) * dir);
                        if (!isEqual(p0.distTo(p1), q0.distTo(q1)) ||
                            !isEqual((p1 - p0).dot(p2 - p0), (q1 - q0).dot(q2 - q0)) ||
                            !isEqual((p1 - p0).prod(p2 - p0), (q1 - q0).prod(q2 - q0) * reflection)) {
                            flag = false;
                        }
                    }
                    if (flag) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    virtual bool isSimilarTo(const Shape& another) const override
    {
        const Polygon* polygon = dynamic_cast<const Polygon*>(&another);
        if (polygon == nullptr || size() != polygon->size()) {
            return false;
        }
        Polygon scaled = *polygon;
        scaled.scale(polygon->points[0], perimeter() / polygon->perimeter());
        return isCongruentTo(scaled);
    }

    virtual bool containsPoint(const Point& p) const override
    {
        double angle = 0.0;
        for (int i = 0; i < size(); ++i) {
            auto& p0 = point(i);
            auto& p1 = point(i + 1);
            if (p == p0 || p == p1) { 
                return true;
            }
            double v = (p - p0).prod(p - p1);
            double s = (p - p0).dot(p - p1);
            double a = acos(s / (p.distTo(p0) * p.distTo(p1)));
            if (v > 0) {
                angle += a;
            } else if (v < 0) {
                angle -= a;
            } else {
                return true; 
            }
        }
        return isEqual(angle, 2 * PI) || isEqual(angle, -2 * PI);
    }

    virtual void rotate(const Point& center, double angle) override
    {
        for (Point& p : points) {
            p.rotate(center, angle);
        }
    }

    virtual void reflex(const Point& center) override
    {
        for (Point& p : points) {
            p.reflect(center);
        }
    }

    virtual void reflex(const Line& axis) override
    {
        for (Point& p : points) {
            p = axis.reflectionOfPoint(p);
        }
    }

    virtual void scale(const Point& center, double coefficient) override
    {
        for (Point& p : points) {
            p.scale(center, coefficient);
        }
    }

    size_t verticesCount() const
    {
        return points.size();
    }

    const vector<Point>& getVertices() const
    {
        return points;
    }

    bool isConvex() const
    {
        if (size() > 3) {
            int dir = (points[1] - points[0]).prod(points[2] - points[1]) > 0 ? 1 : -1;
            for (int i = 0; i < size(); ++i) {
                if ((point(i + dir) - point(i)).prod(point(i + dir * 2) - point(i + dir)) <= 0) {
                    return false;
                }
            }
        }
        return true;
    }

    virtual string className() const override
    {
        return "Polygon";
    }

    virtual string toString() const override
    {
        stringstream ss;
        ss << className() << "{";
        for (auto& p : points) {
            ss << p.toString() << ", ";
        }
        ss << "}";
        return ss.str();
    }

protected:
    int size() const
    {
        return int(points.size());
    }

    const Point& point(int index) const
    {
        return points[(index % size() + size()) % size()];
    }
};

class Rectangle : public Polygon
{
public:
    Rectangle(const Point& p, const Point& q, double k)
    {
        if (k < 1) {
            k = 1. / k;
        }
        Point h1 = p + (q - p) * (k * k / (k * k + 1.));
        Point h2 = p + (q - p) * (1. / (k * k + 1.));
        Point height = (h1 - p) * (1. / k);
        height.rotate(90.);
        points.resize(4);
        points[0] = p;
        points[1] = h1 - height;
        points[2] = q;
        points[3] = h2 + height;
    }

    virtual double perimeter() const override
    {
        return (points[0].distTo(points[1]) + points[1].distTo(points[2])) * 2;
    }

    virtual double area() const override
    {
        return points[0].distTo(points[1]) * points[1].distTo(points[2]);
    }

    Point center() const
    {
        return (points[0] + points[2]) * 0.5;
    }

    pair<Line, Line> diagonals() const
    {
        return { Line(points[0], points[2]), Line(points[1], points[3]) };
    }

    virtual string className() const override
    {
        return "Rectangle";
    }
};

class Square : public Rectangle
{
public:
    Square(const Point& p, const Point& q)
        : Rectangle(p, q, 1.0)
    { }

    Circle circumscribedCircle() const
    {
        return Circle(center(), points[0].distTo(points[2]) * 0.5);
    }
    
    Circle inscribedCircle() const
    {
        return Circle(center(), points[0].distTo(points[1]) * 0.5);
    }

    virtual string className() const override
    {
        return "Square";
    }
};

class Triangle : public Polygon
{
public:
    Triangle(const Point& p0, const Point& p1, const Point& p2)
        : Polygon({ p0, p1, p2 })
    { }

    Circle circumscribedCircle() const
    {
        Point center = medial().orthocenter();
        return Circle(center, center.distTo(points[0]));
    }

    Circle inscribedCircle() const
    {
        double d01 = points[0].distTo(points[1]);
        double d02 = points[0].distTo(points[2]);
        double d12 = points[1].distTo(points[2]);
        Point center = (points[0] * d12 + points[1] * d02 + points[2] * d01) / (d01 + d02 + d12);
        double radius = area() / perimeter() * 2;
        return Circle(center, radius);
    }

    Point centroid() const
    {
        return (points[0] + points[1] + points[2]) / 3.0;
    }

    Point orthocenter() const
    {
        Line l2(Line(points[0], points[1]).projectionOfPoint(points[2]), points[2]);
        Line l0(Line(points[1], points[2]).projectionOfPoint(points[0]), points[0]);
        return l0.intersection(l2);
    }

    Line EulerLine() const
    {
        return Line(centroid(), orthocenter());
    }

    Circle ninePointsCircle() const
    {
        return medial().circumscribedCircle();
    }

    Triangle medial() const
    {
        return Triangle((points[0] + points[1]) * 0.5, (points[1] + points[2]) * 0.5, (points[2] + points[0]) * 0.5);
    }

    virtual string className() const override
    {
        return "Triangle";
    }
};
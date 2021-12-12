#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <tuple>


using I = int; // тип для представления координат
using II = int64_t; // тип для вычисление произведений чисел типа I

struct Fraction
{
    II p;
    I q;

    Fraction(const II& p_, const I& q_)
        : p(q_ < 0 ? -p_ : p_), q(q_ < 0 ? -q_ : q_)
    {
    }

    bool operator<(const Fraction& other) const
    {
        if (q == other.q) {
            return p < other.p;
        } else if (q == 0) {
            return p < 0;
        } else if (other.q == 0) {
            return other.p >= 0;
        }
        II diff = p / q - other.p / other.q;
        if (diff != 0) {
            return diff < 0;
        }
        return p % q * other.q < other.p% other.q* q;
    }

    bool operator==(const Fraction& other) const
    {
        if (q == other.q) {
            return p == other.p;
        } else if (q == 0 || other.q == 0) {
            return false;
        }
        II diff = p / q - other.p / other.q;
        if (diff != 0) {
            return false;
        }
        return p % q * other.q == other.p % other.q * q;
    }
};

struct Line
{
    I m;
    II b;

    Line(const I& m_, const II& b_)
        : m(m_), b(b_)
    {
    }

    bool operator<(const Line& other) const
    {
        return std::tie(m, b) < std::tie(other.m, other.b);
    }

    bool operator>(const Line& other) const
    {
        return std::tie(m, b) > std::tie(other.m, other.b);
    }

    bool operator==(const Line& other) const
    {
        return std::tie(m, b) == std::tie(other.m, other.b);
    }

    Fraction intersect(const Line& other) const
    {
        return Fraction(other.b - b, m - other.m);
    }

    II eval(const I& x) const
    {
        return II(x) * m + b;
    }
};

class LowerEnvelope
{
    std::vector<Line> hull;

    bool reducable(const Line& line) const
    {
        return !(hull.back().intersect(hull[hull.size() - 2]) < hull.back().intersect(line));
    }

public:
    LowerEnvelope(std::vector<Line> lines = std::vector<Line>())
    {
        sort(lines.rbegin(), lines.rend());
        for (const auto& line : lines) {
            add(line);
        }
    }

    void add(const Line& line)
    {
        if (hull.empty()) {
            hull.emplace_back(line);
            return;
        }
        if (line > hull.back()) {
            std::cerr << "invalid order in LowerEnvelope::add\n";
            exit(1);
        } else if (line == hull.back()) {
            return;
        } else if (line.m == hull.back().m) {
            hull.pop_back();
        }
        while (hull.size() >= 3 && reducable(line)) {
            hull.pop_back();
        }
        hull.emplace_back(line);
    }

    II get(const I& x)
    {
        if (hull.empty()) {
            std::cerr << "empty hull in LowerEnvelope::get\n";
            exit(1);
        }
        int l = 0;
        int r = int(hull.size());
        while (r - l > 1) {
            int m = (l + r) / 2;
            if (Fraction(x, 1) < hull[m - 1].intersect(hull[m])) {
                r = m;
            } else {
                l = m;
            }
        }
        return hull[l].eval(x);
    }
};

class UpperEnvelope
{
    std::vector<Line> hull;

    bool reducable(const Line& line) const
    {
        return !(hull.back().intersect(hull[hull.size() - 2]) < hull.back().intersect(line));
    }

public:
    UpperEnvelope(std::vector<Line> lines = std::vector<Line>())
    {
        sort(lines.begin(), lines.end());
        for (const auto& line : lines) {
            add(line);
        }
    }

    void add(const Line& line)
    {
        if (hull.empty()) {
            hull.emplace_back(line);
            return;
        }
        if (line < hull.back()) {
            std::cerr << "invalid order in UpperEnvelope::add\n";
            exit(1);
        } else if (line == hull.back()) {
            return;
        } else if (line.m == hull.back().m) {
            hull.pop_back();
        }
        while (hull.size() >= 2 && reducable(line)) {
            hull.pop_back();
        }
        hull.emplace_back(line);
    }

    II get(const I& x)
    {
        if (hull.empty()) {
            std::cerr << "empty hull in LowerEnvelope::get\n";
            exit(1);
        }
        int l = 0;
        int r = int(hull.size());
        while (r - l > 1) {
            int m = (l + r) / 2;
            if (Fraction(x, 1) < hull[m - 1].intersect(hull[m])) {
                r = m;
            } else {
                l = m;
            }
        }
        return hull[l].eval(x);
    }
};

struct Rect
{
    II x, y, a;

    Rect()
        : x(0), y(0), a(0)
    {
    }

    bool operator<(const Rect& other) const
    {
        return std::tie(x, y, a) < std::tie(other.x, other.y, other.a);
    }
};

II dpSolve(const std::vector<Rect>& V)
{
    int N = int(V.size());
    std::vector<II> ans(N + 1);
    ans[0] = 0;
    for (int i = 0; i < N; ++i) {
        II delta = II(V[i].x) * V[i].y - V[i].a;
        ans[i + 1] = std::max(ans[i], delta);
        for (int j = 1; j <= i; ++j) {
            ans[i + 1] = std::max(ans[i + 1], delta + ans[j] - II(V[j - 1].x) * V[i].y);
        }
    }
    return ans.back();
}

II trickSolve(const std::vector<Rect>& V)
{
    int N = int(V.size());
    UpperEnvelope ue;
    std::vector<II> ans(N + 1);
    ans[0] = 0;
    ue.add(Line(0, 0));
    for (int i = 0; i < N; ++i) {
        II delta = II(V[i].x) * V[i].y - V[i].a;
        ans[i + 1] = std::max(ans[i], delta + ue.get(-V[i].y));
        ue.add(Line(V[i].x, ans[i + 1]));
    }
    return ans.back();
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    std::cin >> N;
    std::vector<Rect> V(N);
    for (int i = 0; i < N; ++i) {
        std::cin >> V[i].x >> V[i].y >> V[i].a;
    }
    sort(V.begin(), V.end());
    std::cout << trickSolve(V) << "\n";
    return 0;
}
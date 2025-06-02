#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>

struct Point { double x, y; };

constexpr double EPS = 1e-9;

/* значение линейной функции на точке */
inline double val(const Point& p, double a, double b, double c)
{ return a * p.x + b * p.y + c; }

/* пересечение отрезка [p,q] с прямой a·x + b·y + c = 0 */
Point intersect(const Point& p, const Point& q,
                double a, double b, double c)
{
    double t = -val(p, a, b, c) /
               (a * (q.x - p.x) + b * (q.y - p.y));
    return {p.x + t * (q.x - p.x), p.y + t * (q.y - p.y)};
}

/* клипуем выпуклый многоугольник полуплоскостью a·x+b·y+c≤0 */
std::vector<Point> clip(const std::vector<Point>& poly,
                        double a, double b, double c)
{
    std::vector<Point> res;
    int m = static_cast<int>(poly.size());
    if (!m) return res;
    for (int i = 0; i < m; ++i) {
        const Point& cur = poly[i];
        const Point& nxt = poly[(i + 1) % m];
        bool in_cur = val(cur, a, b, c) <= EPS;
        bool in_nxt = val(nxt, a, b, c) <= EPS;

        if (in_cur && in_nxt) {
            res.push_back(nxt);
        } else if (in_cur && !in_nxt) {
            res.push_back(intersect(cur, nxt, a, b, c));
        } else if (!in_cur && in_nxt) {
            res.push_back(intersect(cur, nxt, a, b, c));
            res.push_back(nxt);
        }
    }
    return res;
}

/* убираем дубли и коллинеарные вершины */
void tidy(std::vector<Point>& v)
{
    const double LIM = 1e-8;
    auto dist2 = [](const Point& p, const Point& q)
    { double dx = p.x - q.x, dy = p.y - q.y; return dx*dx + dy*dy; };

    std::vector<Point> t;
    for (auto& p: v)
        if (t.empty() || dist2(p, t.back()) > LIM) t.push_back(p);
    if (t.size() > 1 && dist2(t.front(), t.back()) < LIM) t.pop_back();
    v.swap(t);

    t.clear();
    int m = static_cast<int>(v.size());
    for (int i = 0; i < m; ++i) {
        Point a = v[(i + m - 1) % m], b = v[i], c = v[(i + 1) % m];
        double cr = (b.x - a.x)*(c.y - b.y) - (b.y - a.y)*(c.x - b.x);
        if (std::fabs(cr) > 1e-10) t.push_back(b);
    }
    v.swap(t);
}

/* начинаем обход с самой нижней среди самых левых вершин */
void rotate_start(std::vector<Point>& poly)
{
    if (poly.empty()) return;
    int best = 0;
    for (int i = 1; i < (int)poly.size(); ++i) {
        if (poly[i].x < poly[best].x - EPS ||
           (std::fabs(poly[i].x - poly[best].x) < EPS &&
            poly[i].y < poly[best].y - EPS))
            best = i;
    }
    std::rotate(poly.begin(), poly.begin() + best, poly.end());
}

std::vector<std::vector<Point>>
solve(int X, int Y, const std::vector<Point>& p)
{
    int n = static_cast<int>(p.size());
    std::vector<std::vector<Point>> cells(n);
    std::vector<Point> rect = {
        {0,0}, {static_cast<double>(X),0},
        {static_cast<double>(X),static_cast<double>(Y)},
        {0,static_cast<double>(Y)}
    };

    for (int i = 0; i < n; ++i) {
        std::vector<Point> poly = rect;
        for (int j = 0; j < n && !poly.empty(); ++j) if (j != i) {
            double a = 2 * (p[j].x - p[i].x);
            double b = 2 * (p[j].y - p[i].y);
            double c = p[i].x*p[i].x + p[i].y*p[i].y
                     - p[j].x*p[j].x - p[j].y*p[j].y;
            poly = clip(poly, a, b, c);
            tidy(poly);
        }
        rotate_start(poly);
        cells[i] = std::move(poly);
    }
    return cells;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int X, Y, n;
    std::cin >> X >> Y >> n;
    std::vector<Point> p(n);
    for (auto& pt : p) std::cin >> pt.x >> pt.y;

    auto zones = solve(X, Y, p);

    std::cout << std::fixed << std::setprecision(8);
    for (const auto& poly : zones) {
        std::cout << poly.size();
        for (const auto& v : poly) std::cout << ' ' << v.x << ' ' << v.y;
        std::cout << '\n';
    }
    return 0;
}

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

struct Point {
    std::int64_t x;
    std::int64_t y;
};

using i128 = __int128_t;

i128 cross(const Point& a, const Point& b, const Point& c) {
    return static_cast<i128>(b.x - a.x) * (c.y - a.y) -
           static_cast<i128>(b.y - a.y) * (c.x - a.x);
}

bool cmp_xy(const Point& a, const Point& b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

bool on_segment(const Point& a, const Point& b, const Point& q) {
    if (cross(a, b, q) != 0) return false;
    return (q.x - a.x) * (q.x - b.x) <= 0 &&
           (q.y - a.y) * (q.y - b.y) <= 0;
}

std::vector<Point> convex_hull(std::vector<Point>& pts) {
    std::sort(pts.begin(), pts.end(), cmp_xy);
    pts.erase(std::unique(pts.begin(), pts.end(),
                          [](const Point& p, const Point& q) {
                              return p.x == q.x && p.y == q.y;
                          }),
              pts.end());
    int n = static_cast<int>(pts.size());
    if (n <= 1) return pts;
    std::vector<Point> lower, upper;
    for (const auto& p : pts) {
        while (lower.size() >= 2 &&
               cross(lower[lower.size() - 2], lower.back(), p) <= 0)
            lower.pop_back();
        lower.push_back(p);
    }
    for (int i = n - 1; i >= 0; --i) {
        const auto& p = pts[i];
        while (upper.size() >= 2 &&
               cross(upper[upper.size() - 2], upper.back(), p) <= 0)
            upper.pop_back();
        upper.push_back(p);
    }
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

bool inside_convex(const std::vector<Point>& P, const Point& q) {
    int n = static_cast<int>(P.size());
    if (n == 0) return false;
    if (n == 1) return P[0].x == q.x && P[0].y == q.y;
    if (n == 2) return on_segment(P[0], P[1], q);
    if (cross(P[0], P[1], q) < 0) return false;
    if (cross(P[0], P[n - 1], q) > 0) return false;
    int l = 1, r = n - 1;
    while (r - l > 1) {
        int m = (l + r) >> 1;
        if (cross(P[0], P[m], q) >= 0)
            l = m;
        else
            r = m;
    }
    return cross(P[l], P[r % n], q) >= 0;
}

std::vector<std::vector<Point>> build_layers(const std::vector<Point>& src) {
    std::vector<Point> rest = src;
    std::vector<std::vector<Point>> layers;
    while (!rest.empty()) {
        std::vector<Point> hull;
        if (rest.size() >= 3)
            hull = convex_hull(rest);
        else
            hull = rest;
        layers.push_back(hull);
        std::vector<Point> next;
        for (const auto& p : rest) {
            bool on_hull = false;
            for (const auto& h : hull)
                if (h.x == p.x && h.y == p.y) { on_hull = true; break; }
            if (!on_hull && hull.size() >= 2) {
                int m = static_cast<int>(hull.size());
                for (int i = 0; i < m && !on_hull; ++i) {
                    const Point& a = hull[i];
                    const Point& b = hull[(i + 1) % m];
                    if (on_segment(a, b, p)) on_hull = true;
                }
            }
            if (!on_hull) next.push_back(p);
        }
        rest.swap(next);
    }
    return layers;
}

void solve() {
    int N;
    std::cin >> N;
    std::vector<Point> attractions(N);
    for (auto& p : attractions) std::cin >> p.x >> p.y;
    int K;
    std::cin >> K;
    std::vector<Point> queries(K);
    for (auto& q : queries) std::cin >> q.x >> q.y;
    auto layers = build_layers(attractions);
    for (const auto& q : queries) {
        int zone = -1;
        for (std::size_t i = 0; i < layers.size(); ++i) {
            if (inside_convex(layers[i], q))
                zone = static_cast<int>(i);
            else
                break;
        }
        std::cout << (zone < 0 ? 0 : zone) << '\n';
    }
}

int main() { solve(); }

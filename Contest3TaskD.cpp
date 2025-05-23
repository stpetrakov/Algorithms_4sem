#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

struct Point {
    long double x, y;
};

using Polygon = std::vector<Point>;

static long double cross(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}
static Point operator+(const Point& a, const Point& b) {
    return { a.x + b.x, a.y + b.y };
}
static Point operator-(const Point& a, const Point& b) {
    return { a.x - b.x, a.y - b.y };
}

struct Data {
    Polygon airport;
    Polygon cloud;
};

static Data read_input() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    Polygon A(n), B(m);
    for (auto& p : A) std::cin >> p.x >> p.y;
    for (auto& p : B) std::cin >> p.x >> p.y;
    return { std::move(A), std::move(B) };
}

static void write_output(long double sec) {
    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(10) << static_cast<double>(sec);
}

static int lex_lowest(const Polygon& P) {
    int idx = 0;
    for (int i = 1; i < (int)P.size(); ++i)
        if (P[i].y < P[idx].y || (P[i].y == P[idx].y && P[i].x < P[idx].x))
            idx = i;
    return idx;
}

static Polygon minkowski_sum(const Polygon& A, const Polygon& B) {
    int n = static_cast<int>(A.size());
    int m = static_cast<int>(B.size());
    int ia = lex_lowest(A);
    int ib = lex_lowest(B);

    Polygon C;
    C.reserve(n + m);

    int i = 0, j = 0;
    Point cur = A[ia] + B[ib];
    C.push_back(cur);

    while (i < n || j < m) {
        Point va = A[(ia + 1) % n] - A[ia];
        Point vb = B[(ib + 1) % m] - B[ib];

        long double z = (i == n ? 0 : (j == m ? 0 : cross(va, vb)));
        if (j == m || (i < n && z > 0)) {
            ia = (ia + 1) % n;
            cur = cur + va;
            ++i;
        } else if (i == n || z < 0) {
            ib = (ib + 1) % m;
            cur = cur + vb;
            ++j;
        } else {
            ia = (ia + 1) % n;
            ib = (ib + 1) % m;
            cur = cur + va + vb;
            ++i; ++j;
        }
        C.push_back(cur);
    }
    C.pop_back();
    return C;
}

static long double dist_origin_to_edge(const Point& a, const Point& b) {
    long double area2 = fabsl(cross(a, b));
    long double len = std::hypotl(b.x - a.x, b.y - a.y);
    return area2 / len;
}

static long double min_distance_to_boundary(const Polygon& P) {
    long double best = 1e100L;
    int n = static_cast<int>(P.size());
    for (int i = 0; i < n; ++i) {
        const Point& a = P[i];
        const Point& b = P[(i + 1) % n];
        best = std::min(best, dist_origin_to_edge(a, b));
    }
    return best;
}

static long double solve(const Data& d) {
    Polygon inv_cloud = d.cloud;
    for (auto& p : inv_cloud) { p.x = -p.x; p.y = -p.y; }

    Polygon mink = minkowski_sum(d.airport, inv_cloud);

    long double dist = min_distance_to_boundary(mink);
    const long double passed = 60.0L;
    if (dist <= passed + 1e-12L) return 0.0L;
    return dist - passed;
}

int main() {
    Data data = read_input();
    long double ans = solve(data);
    write_output(ans);
    return 0;
}

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>


struct Vec3 {
  double x, y, z;
  Vec3 operator+(const Vec3 &o) const { return {x + o.x, y + o.y, z + o.z}; }
  Vec3 operator-(const Vec3 &o) const { return {x - o.x, y - o.y, z - o.z}; }
  Vec3 operator/(double k) const { return {x / k, y / k, z / k}; }
};

double dot(const Vec3 &a, const Vec3 &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3 &a, const Vec3 &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

struct Face {
  int a, b, c;
  Vec3 n;
  double d;
  bool dead = false;
};

struct Plane {
  Vec3 n;
  double len;
  double d;
};

constexpr double EPS = 1e-9;

struct InputData {
  std::vector<Vec3> points;
  std::vector<Vec3> queries;
};

InputData read_input() {
  int n;
  std::cin >> n;
  std::vector<Vec3> pts(n);
  for (auto &p : pts)
    std::cin >> p.x >> p.y >> p.z;

  int q;
  std::cin >> q;
  std::vector<Vec3> queries(q);
  for (auto &v : queries)
    std::cin >> v.x >> v.y >> v.z;

  return {std::move(pts), std::move(queries)};
}

void write_output(const std::vector<double> &ans) {
  std::cout.setf(std::ios::fixed);
  std::cout << std::setprecision(9);
  for (double v : ans)
    std::cout << v << '\n';
}

inline uint64_t edge_key(int u, int v) {
  return (static_cast<uint64_t>(std::min(u, v)) << 32) |
         static_cast<uint32_t>(std::max(u, v));
}

void add_face(std::vector<Face> &faces, int a, int b, int c,
              const std::vector<Vec3> &pts, const Vec3 &inner_point) {
  Vec3 n = cross(pts[b] - pts[a], pts[c] - pts[a]);
  double d = -dot(n, pts[a]);

  if (dot(n, inner_point) + d > 0) {
    std::swap(b, c);
    n = {-n.x, -n.y, -n.z};
    d = -d;
  }
  faces.push_back({a, b, c, n, d});
}

void find_initial_tetra(const std::vector<Vec3> &p, int &i0, int &i1, int &i2,
                        int &i3) {
  const int n = static_cast<int>(p.size());
  i0 = 0;
  for (i1 = 1; i1 < n; ++i1)
    if (p[i1].x != p[i0].x || p[i1].y != p[i0].y || p[i1].z != p[i0].z)
      break;

  for (i2 = i1 + 1; i2 < n; ++i2) {
    Vec3 cp = cross(p[i1] - p[i0], p[i2] - p[i0]);
    if (std::fabs(cp.x) + std::fabs(cp.y) + std::fabs(cp.z) > EPS)
      break;
  }

  Vec3 base_n = cross(p[i1] - p[i0], p[i2] - p[i0]);
  for (i3 = i2 + 1; i3 < n; ++i3)
    if (std::fabs(dot(p[i3] - p[i0], base_n)) > EPS)
      break;
}

std::vector<Plane> build_convex_hull(const std::vector<Vec3> &pts) {
  int n = static_cast<int>(pts.size());
  int i0, i1, i2, i3;
  find_initial_tetra(pts, i0, i1, i2, i3);

  std::vector<Face> faces;
  Vec3 inner = (pts[i0] + pts[i1] + pts[i2] + pts[i3]) / 4.0;

  add_face(faces, i0, i1, i2, pts, inner);
  add_face(faces, i0, i3, i1, pts, inner);
  add_face(faces, i0, i2, i3, pts, inner);
  add_face(faces, i1, i3, i2, pts, inner);

  for (int idx = 0; idx < n; ++idx) {
    if (idx == i0 || idx == i1 || idx == i2 || idx == i3)
      continue;

    std::vector<int> visible;
    for (int f = 0; f < static_cast<int>(faces.size()); ++f)
      if (!faces[f].dead && dot(faces[f].n, pts[idx]) + faces[f].d > EPS)
        visible.push_back(f);

    if (visible.empty())
      continue;

    std::unordered_map<uint64_t, int> edgeCnt;
    for (int fid : visible) {
      Face &fc = faces[fid];
      fc.dead = true;

      int v[3] = {fc.a, fc.b, fc.c};
      for (int e = 0; e < 3; ++e) {
        uint64_t key = edge_key(v[e], v[(e + 1) % 3]);
        ++edgeCnt[key];
      }
    }

    for (const auto &kv : edgeCnt)
      if (kv.second == 1) {
        int u = static_cast<int>(kv.first >> 32);
        int v = static_cast<int>(kv.first & 0xffffffffu);
        add_face(faces, u, v, idx, pts, inner);
      }
  }

  std::vector<Plane> pl;
  for (const auto &f : faces)
    if (!f.dead) {
      double len = std::sqrt(dot(f.n, f.n));
      pl.push_back({f.n, len, f.d});
    }
  return pl;
}

double distance_to_hull_point(const std::vector<Plane> &pl, const Vec3 &q) {
  double best = std::numeric_limits<double>::max();
  for (const auto &pln : pl) {
    double signed_dist = dot(pln.n, q) + pln.d;
    double dist = -signed_dist / pln.len;
    if (dist < best)
      best = dist;
  }
  return best;
}

std::vector<double> answer_queries(const std::vector<Plane> &pl,
                                   const std::vector<Vec3> &queries) {
  std::vector<double> ans;
  ans.reserve(queries.size());
  for (const auto &q : queries)
    ans.push_back(distance_to_hull_point(pl, q));
  return ans;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  InputData data = read_input();
  std::vector<Plane> hull_planes = build_convex_hull(data.points);
  std::vector<double> ans = answer_queries(hull_planes, data.queries);
  write_output(ans);
  return 0;
}

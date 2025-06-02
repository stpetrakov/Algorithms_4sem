#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

namespace math3d {

constexpr double kEpsilon = 1e-9;

struct Vector3 {
  double x{};
  double y{};
  double z{};

  [[nodiscard]] Vector3 operator+(const Vector3 &other) const noexcept {
    return {x + other.x, y + other.y, z + other.z};
  }
  [[nodiscard]] Vector3 operator-(const Vector3 &other) const noexcept {
    return {x - other.x, y - other.y, z - other.z};
  }
  [[nodiscard]] Vector3 operator/(double k) const noexcept {
    return {x / k, y / k, z / k};
  }
};

[[nodiscard]] inline double Dot(const Vector3 &a, const Vector3 &b) noexcept {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[nodiscard]] inline Vector3 Cross(const Vector3 &a, const Vector3 &b) noexcept {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

}

struct InputData {
  std::vector<math3d::Vector3> points;
  std::vector<math3d::Vector3> queries;
};

InputData ReadInput();
void WriteOutput(const std::vector<double> &answers);

namespace hull {

using math3d::Cross;
using math3d::Dot;
using math3d::Vector3;
using math3d::kEpsilon;
using VertexIndex = int;

struct Face {
  VertexIndex vertexA{0};
  VertexIndex vertexB{0};
  VertexIndex vertexC{0};
  Vector3 normal;
  double offset{0.0};
  bool isDeleted{false};
};

struct Plane {
  Vector3 normal;
  double normalLength{1.0};
  double offset{0.0};
};

constexpr uint64_t MakeEdgeKey(VertexIndex u, VertexIndex v) noexcept {
  return (static_cast<uint64_t>(std::min(u, v)) << 32) |
         static_cast<uint32_t>(std::max(u, v));
}

std::array<VertexIndex, 4> FindInitialTetrahedron(const std::vector<Vector3> &pts) {
  const int n = static_cast<int>(pts.size());
  VertexIndex v0 = 0, v1 = 0, v2 = 0, v3 = 0;

  for (v1 = 1; v1 < n; ++v1)
    if (pts[v1].x != pts[v0].x || pts[v1].y != pts[v0].y || pts[v1].z != pts[v0].z)
      break;

  for (v2 = v1 + 1; v2 < n; ++v2) {
    const auto cp = Cross(pts[v1] - pts[v0], pts[v2] - pts[v0]);
    if (std::fabs(cp.x) + std::fabs(cp.y) + std::fabs(cp.z) > kEpsilon)
      break;
  }

  const auto baseNormal = Cross(pts[v1] - pts[v0], pts[v2] - pts[v0]);
  for (v3 = v2 + 1; v3 < n; ++v3)
    if (std::fabs(Dot(pts[v3] - pts[v0], baseNormal)) > kEpsilon)
      break;

  return {v0, v1, v2, v3};
}

void AddFace(std::vector<Face> &faces, VertexIndex va, VertexIndex vb, VertexIndex vc,
             const std::vector<Vector3> &pts, const Vector3 &innerPoint) {
  Vector3 normal = Cross(pts[vb] - pts[va], pts[vc] - pts[va]);
  double offset = -Dot(normal, pts[va]);

  if (Dot(normal, innerPoint) + offset > 0) {
    std::swap(vb, vc);
    normal = {-normal.x, -normal.y, -normal.z};
    offset = -offset;
  }
  faces.push_back({va, vb, vc, normal, offset, false});
}

std::vector<VertexIndex> CollectVisibleFaces(const std::vector<Face> &faces,
                                             const std::vector<Vector3> &pts,
                                             VertexIndex pointId) {
  std::vector<VertexIndex> visible;
  for (VertexIndex faceId = 0; faceId < static_cast<VertexIndex>(faces.size()); ++faceId) {
    const Face &face = faces[faceId];
    if (!face.isDeleted && Dot(face.normal, pts[pointId]) + face.offset > kEpsilon)
      visible.push_back(faceId);
  }
  return visible;
}

std::unordered_map<uint64_t, int> MarkFacesDeletedAndCollectBorder(
    std::vector<Face> &faces, const std::vector<VertexIndex> &visibleFaces) {
  std::unordered_map<uint64_t, int> borderUsage;
  for (VertexIndex faceId : visibleFaces) {
    Face &face = faces[faceId];
    face.isDeleted = true;
    const std::array<VertexIndex, 3> vertices = {face.vertexA, face.vertexB, face.vertexC};
    for (int edgeIndex = 0; edgeIndex < 3; ++edgeIndex) {
      uint64_t key = MakeEdgeKey(vertices[edgeIndex], vertices[(edgeIndex + 1) % 3]);
      ++borderUsage[key];
    }
  }
  return borderUsage;
}

void AddHorizonFaces(std::vector<Face> &faces,
                     const std::unordered_map<uint64_t, int> &borderUsage,
                     VertexIndex newPoint,
                     const std::vector<Vector3> &pts,
                     const Vector3 &innerPoint) {
  for (const auto &[key, count] : borderUsage) {
    if (count != 1)
      continue;
    VertexIndex u = static_cast<VertexIndex>(key >> 32);
    VertexIndex v = static_cast<VertexIndex>(key & 0xffffffffu);
    AddFace(faces, u, v, newPoint, pts, innerPoint);
  }
}

std::vector<Plane> BuildConvexHull(const std::vector<Vector3> &pts) {
  const auto tetra = FindInitialTetrahedron(pts);
  const VertexIndex v0 = tetra[0], v1 = tetra[1], v2 = tetra[2], v3 = tetra[3];

  std::vector<Face> faces;
  const Vector3 innerPoint = (pts[v0] + pts[v1] + pts[v2] + pts[v3]) / 4.0;

  AddFace(faces, v0, v1, v2, pts, innerPoint);
  AddFace(faces, v0, v3, v1, pts, innerPoint);
  AddFace(faces, v0, v2, v3, pts, innerPoint);
  AddFace(faces, v1, v3, v2, pts, innerPoint);

  const int n = static_cast<int>(pts.size());
  for (VertexIndex p = 0; p < n; ++p) {
    if (p == v0 || p == v1 || p == v2 || p == v3)
      continue;

    auto visibleFaces = CollectVisibleFaces(faces, pts, p);
    if (visibleFaces.empty())
      continue;

    auto borderUsage = MarkFacesDeletedAndCollectBorder(faces, visibleFaces);
    AddHorizonFaces(faces, borderUsage, p, pts, innerPoint);
  }

  std::vector<Plane> planes;
  planes.reserve(faces.size());
  for (const auto &face : faces) {
    if (face.isDeleted)
      continue;
    double length = std::sqrt(Dot(face.normal, face.normal));
    planes.push_back({face.normal, length, face.offset});
  }
  return planes;
}

double DistanceToHull(const std::vector<Plane> &planes, const Vector3 &query) {
  double minDistance = std::numeric_limits<double>::max();
  for (const auto &plane : planes) {
    double signedDist = Dot(plane.normal, query) + plane.offset;
    double dist = -signedDist / plane.normalLength;
    minDistance = std::min(minDistance, dist);
  }
  return minDistance;
}

std::vector<double> AnswerQueries(const std::vector<Plane> &planes,
                                  const std::vector<Vector3> &queries) {
  std::vector<double> result;
  result.reserve(queries.size());
  for (const auto &q : queries)
    result.push_back(DistanceToHull(planes, q));
  return result;
}

}

InputData ReadInput() {
  using math3d::Vector3;
  std::size_t pointCount;
  std::cin >> pointCount;
  std::vector<Vector3> points(pointCount);
  for (auto &p : points)
    std::cin >> p.x >> p.y >> p.z;

  std::size_t queryCount;
  std::cin >> queryCount;
  std::vector<Vector3> queries(queryCount);
  for (auto &q : queries)
    std::cin >> q.x >> q.y >> q.z;

  return {std::move(points), std::move(queries)};
}

void WriteOutput(const std::vector<double> &answers) {
  std::cout.setf(std::ios::fixed);
  std::cout << std::setprecision(9);
  for (double v : answers)
    std::cout << v << '\n';
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  const InputData data = ReadInput();
  const auto planes = hull::BuildConvexHull(data.points);
  const auto distances = hull::AnswerQueries(planes, data.queries);
  WriteOutput(distances);
  return 0;
}

#pragma once
#include <QtCore/QPointF>
#include <vector>
#include <optional>

namespace geom {

struct Segment {
    QPointF a;
    QPointF b;
};

// Returns +1 if p is left of ab, -1 if right, 0 if on segment
int pointRelativeToSegment(const QPointF& a, const QPointF& b, const QPointF& p, double eps = 1e-9);

// Segment intersection result: empty none, point, or segment overlap
struct Intersection {
    enum class Type { None, Point, Segment } type = Type::None;
    QPointF p1; // for point or start of overlap
    QPointF p2; // end of overlap if Segment
};

Intersection intersectSegments(const Segment& s1, const Segment& s2, double eps = 1e-9);

// Convex hull (Monotone chain), returns points in CCW without repeating first
std::vector<QPointF> convexHull(std::vector<QPointF> pts, double eps = 1e-9);

// Utility
double cross(const QPointF& a, const QPointF& b, const QPointF& c);

// Point in convex polygon: returns -1 outside, 0 on boundary (<=eps), 1 inside
int pointInConvexPolygon(const std::vector<QPointF>& polyCCW, const QPointF& p, double eps = 1e-9);

// Point in arbitrary polygon using even-odd rule: -1 outside, 0 on boundary, 1 inside
int pointInPolygon(const std::vector<QPointF>& poly, const QPointF& p, double eps = 1e-9);

// Delaunay triangulation (Bowyer-Watson); returns triples of point indices (counter-clockwise)
struct Triangle { int a, b, c; };
std::vector<Triangle> delaunayTriangulate(const std::vector<QPointF>& pts, double eps = 1e-9);

}



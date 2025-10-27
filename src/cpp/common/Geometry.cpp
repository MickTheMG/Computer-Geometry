#include "Geometry.h"
#include <algorithm>
#include <cmath>

namespace geom {

double cross(const QPointF& a, const QPointF& b, const QPointF& c) {
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}

static bool almostEqual(double x, double y, double eps) {
    return std::fabs(x - y) <= eps;
}

static int sgn(double x, double eps) {
    if (x > eps) return 1;
    if (x < -eps) return -1;
    return 0;
}

int pointRelativeToSegment(const QPointF& a, const QPointF& b, const QPointF& p, double eps) {
    double cr = cross(a, b, p);
    int c = sgn(cr, eps);
    if (c != 0) return c; // left or right
    // Collinear: check if p lies on segment
    double minx = std::min(a.x(), b.x()) - eps, maxx = std::max(a.x(), b.x()) + eps;
    double miny = std::min(a.y(), b.y()) - eps, maxy = std::max(a.y(), b.y()) + eps;
    if (p.x() >= minx && p.x() <= maxx && p.y() >= miny && p.y() <= maxy) return 0;
    return 0;
}

static bool onSegment(const QPointF& a, const QPointF& b, const QPointF& p, double eps) {
    if (sgn(cross(a, b, p), eps) != 0) return false;
    return p.x() >= std::min(a.x(), b.x()) - eps && p.x() <= std::max(a.x(), b.x()) + eps &&
           p.y() >= std::min(a.y(), b.y()) - eps && p.y() <= std::max(a.y(), b.y()) + eps;
}

Intersection intersectSegments(const Segment& s1, const Segment& s2, double eps) {
    const QPointF &a = s1.a, &b = s1.b, &c = s2.a, &d = s2.b;
    double d1 = cross(a, b, c);
    double d2 = cross(a, b, d);
    double d3 = cross(c, d, a);
    double d4 = cross(c, d, b);

    int s1c = sgn(d1, eps), s1d = sgn(d2, eps), s2a = sgn(d3, eps), s2b = sgn(d4, eps);

    Intersection res;

    if (s1c == 0 && s1d == 0 && s2a == 0 && s2b == 0) {
        // Collinear; sort by x then y and find overlap
        auto lessXY = [](const QPointF& p1, const QPointF& p2){
            if (p1.x() < p2.x()) return true;
            if (p1.x() > p2.x()) return false;
            return p1.y() < p2.y();
        };
        QPointF A = a, B = b, C = c, D = d;
        if (lessXY(B, A)) std::swap(A, B);
        if (lessXY(D, C)) std::swap(C, D);
        QPointF lo = lessXY(A, C) ? C : A;
        QPointF hi = lessXY(B, D) ? B : D;
        if (lessXY(lo, hi) || (almostEqual(lo.x(), hi.x(), eps) && almostEqual(lo.y(), hi.y(), eps))) {
            if (almostEqual(lo.x(), hi.x(), eps) && almostEqual(lo.y(), hi.y(), eps)) {
                res.type = Intersection::Type::Point;
                res.p1 = lo;
            } else {
                res.type = Intersection::Type::Segment;
                res.p1 = lo; res.p2 = hi;
            }
        }
        return res;
    }

    if (s1c == 0 && onSegment(a, b, c, eps)) { res.type = Intersection::Type::Point; res.p1 = c; return res; }
    if (s1d == 0 && onSegment(a, b, d, eps)) { res.type = Intersection::Type::Point; res.p1 = d; return res; }
    if (s2a == 0 && onSegment(c, d, a, eps)) { res.type = Intersection::Type::Point; res.p1 = a; return res; }
    if (s2b == 0 && onSegment(c, d, b, eps)) { res.type = Intersection::Type::Point; res.p1 = b; return res; }

    if (((s1c>0 && s1d<0) || (s1c<0 && s1d>0)) && ((s2a>0 && s2b<0) || (s2a<0 && s2b>0))) {
        // Proper intersection; compute intersection point of lines
        QPointF r(b.x()-a.x(), b.y()-a.y());
        QPointF s(d.x()-c.x(), d.y()-c.y());
        double rxs = r.x()*s.y() - r.y()*s.x();
        QPointF qp(c.x()-a.x(), c.y()-a.y());
        double t = (qp.x()*s.y() - qp.y()*s.x()) / rxs;
        res.type = Intersection::Type::Point;
        res.p1 = QPointF(a.x() + t*r.x(), a.y() + t*r.y());
        return res;
    }

    return res; // None
}

std::vector<QPointF> convexHull(std::vector<QPointF> pts, double eps) {
    if (pts.size() <= 1) return pts;
    std::sort(pts.begin(), pts.end(), [](const QPointF& p1, const QPointF& p2){
        if (p1.x() == p2.x()) return p1.y() < p2.y();
        return p1.x() < p2.x();
    });

    auto cross2 = [](const QPointF& o, const QPointF& a, const QPointF& b){
        return (a.x()-o.x())*(b.y()-o.y()) - (a.y()-o.y())*(b.x()-o.x());
    };

    std::vector<QPointF> lower, upper;
    for (const auto& p : pts) {
        while (lower.size() >= 2 && cross2(lower[lower.size()-2], lower.back(), p) <= eps) lower.pop_back();
        lower.push_back(p);
    }
    for (auto it = pts.rbegin(); it != pts.rend(); ++it) {
        const auto& p = *it;
        while (upper.size() >= 2 && cross2(upper[upper.size()-2], upper.back(), p) <= eps) upper.pop_back();
        upper.push_back(p);
    }
    if (!lower.empty()) lower.pop_back();
    if (!upper.empty()) upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

} // namespace geom

// Additional algorithms implementation
namespace geom {

int pointInConvexPolygon(const std::vector<QPointF>& poly, const QPointF& p, double eps) {
    if (poly.size() < 3) return -1;
    int s = 0; // sign of cross products
    for (size_t i = 0; i < poly.size(); ++i) {
        const QPointF& a = poly[i];
        const QPointF& b = poly[(i+1)%poly.size()];
        double c = cross(a, b, p);
        if (std::fabs(c) <= eps) {
            // on edge?
            if (p.x() >= std::min(a.x(), b.x()) - eps && p.x() <= std::max(a.x(), b.x()) + eps &&
                p.y() >= std::min(a.y(), b.y()) - eps && p.y() <= std::max(a.y(), b.y()) + eps) return 0;
        } else {
            int sc = c > 0 ? 1 : -1;
            if (s == 0) s = sc; else if (s != sc) return -1;
        }
    }
    return 1;
}

int pointInPolygon(const std::vector<QPointF>& poly, const QPointF& p, double eps) {
    bool inside = false;
    for (size_t i = 0, j = poly.size() - 1; i < poly.size(); j = i++) {
        const QPointF& a = poly[i];
        const QPointF& b = poly[j];
        // boundary check
        double cr = cross(a, b, p);
        if (std::fabs(cr) <= eps &&
            p.x() >= std::min(a.x(), b.x()) - eps && p.x() <= std::max(a.x(), b.x()) + eps &&
            p.y() >= std::min(a.y(), b.y()) - eps && p.y() <= std::max(a.y(), b.y()) + eps) return 0;
        bool cond = ((a.y() > p.y()) != (b.y() > p.y())) &&
                    (p.x() < (b.x() - a.x()) * (p.y() - a.y()) / (b.y() - a.y() + 1e-30) + a.x());
        if (cond) inside = !inside;
    }
    return inside ? 1 : -1;
}

struct Edge { int u, v; };

static bool circumcircleContains(const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& p) {
    // Robust inCircle predicate with orientation compensation
    double ax = a.x() - p.x(); double ay = a.y() - p.y();
    double bx = b.x() - p.x(); double by = b.y() - p.y();
    double cx = c.x() - p.x(); double cy = c.y() - p.y();
    double det = (ax*ax + ay*ay) * (bx*cy - by*cx)
               - (bx*bx + by*by) * (ax*cy - ay*cx)
               + (cx*cx + cy*cy) * (ax*by - ay*bx);
    // Orientation of triangle (a,b,c)
    double orient = (b.x()-a.x())*(c.y()-a.y()) - (b.y()-a.y())*(c.x()-a.x());
    if (orient > 0) return det > 0; // CCW
    if (orient < 0) return det < 0; // CW
    return false; // degeneracy
}

std::vector<Triangle> delaunayTriangulate(const std::vector<QPointF>& points, double eps) {
    Q_UNUSED(eps);
    std::vector<QPointF> pts = points;
    std::vector<Triangle> tris;
    if (pts.size() < 3) return tris;

    // Super triangle large enough
    double minx = pts[0].x(), miny = pts[0].y(), maxx = minx, maxy = miny;
    for (const auto& p : pts) { minx = std::min(minx, p.x()); miny = std::min(miny, p.y()); maxx = std::max(maxx, p.x()); maxy = std::max(maxy, p.y()); }
    double dx = maxx - minx, dy = maxy - miny; double dmax = std::max(dx, dy);
    QPointF p1(minx - 10*dmax, miny - dmax), p2(minx + 0.5*dmax, maxy + 10*dmax), p3(maxx + 10*dmax, miny - dmax);
    int i1 = (int)pts.size(); pts.push_back(p1);
    int i2 = (int)pts.size(); pts.push_back(p2);
    int i3 = (int)pts.size(); pts.push_back(p3);
    tris.push_back({i1,i2,i3});

    for (int pi = 0; pi < (int)points.size(); ++pi) {
        const QPointF& p = points[pi];
        std::vector<Edge> edges;
        // Mark triangles to remove
        std::vector<bool> bad(tris.size(), false);
        for (size_t t = 0; t < tris.size(); ++t) {
            const Triangle& tr = tris[t];
            const QPointF &a = pts[tr.a], &b = pts[tr.b], &c = pts[tr.c];
            if (circumcircleContains(a,b,c,p)) {
                bad[t] = true;
                edges.push_back({tr.a, tr.b});
                edges.push_back({tr.b, tr.c});
                edges.push_back({tr.c, tr.a});
            }
        }
        // Remove duplicate edges (edges shared twice)
        auto same = [](const Edge& e1, const Edge& e2){ return (e1.u==e2.v && e1.v==e2.u) || (e1.u==e2.u && e1.v==e2.v); };
        std::vector<Edge> boundary;
        for (size_t i = 0; i < edges.size(); ++i) {
            bool dup = false;
            for (size_t j = 0; j < edges.size(); ++j) if (i != j && same(edges[i], edges[j])) { dup = true; break; }
            if (!dup) boundary.push_back(edges[i]);
        }
        // Erase bad triangles
        std::vector<Triangle> kept; kept.reserve(tris.size());
        for (size_t t = 0; t < tris.size(); ++t) if (!bad[t]) kept.push_back(tris[t]);
        tris.swap(kept);
        // Create new triangles with point p index pi
        for (const auto& e : boundary) tris.push_back({e.u, e.v, pi});
    }

    // Remove triangles that use super-triangle vertices
    std::vector<Triangle> out; out.reserve(tris.size());
    for (const auto& tr : tris) if (tr.a < (int)points.size() && tr.b < (int)points.size() && tr.c < (int)points.size()) out.push_back(tr);
    return out;
}

} // namespace geom



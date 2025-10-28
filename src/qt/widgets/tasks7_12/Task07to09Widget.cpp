#include "Task07to09Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <cmath>

static double cross2d(const QPointF& a, const QPointF& b, const QPointF& c) {
    return (b.x()-a.x())*(c.y()-a.y()) - (b.y()-a.y())*(c.x()-a.x());
}

static bool segmentIntersection(const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& d, QPointF& out) {
    double a1 = b.y() - a.y();
    double b1 = a.x() - b.x();
    double c1 = a1 * a.x() + b1 * a.y();

    double a2 = d.y() - c.y();
    double b2 = c.x() - d.x();
    double c2 = a2 * c.x() + b2 * c.y();

    double det = a1*b2 - a2*b1;
    if (std::fabs(det) < 1e-12) return false;
    double x = (b2*c1 - b1*c2) / det;
    double y = (a1*c2 - a2*c1) / det;
    out = QPointF(x, y);
    return true;
}

static std::vector<QPointF> clipWithEdge(const std::vector<QPointF>& subject, const QPointF& a, const QPointF& b) {
    std::vector<QPointF> out; out.reserve(subject.size()+4);
    if (subject.empty()) return out;
    auto inside = [&](const QPointF& p){ return cross2d(a, b, p) >= -1e-9; };
    auto intersect = [&](const QPointF& s, const QPointF& e){ QPointF p; segmentIntersection(s, e, a, b, p); return p; };

    QPointF S = subject.back();
    bool S_in = inside(S);
    for (const QPointF& E : subject) {
        bool E_in = inside(E);
        if (E_in) {
            if (!S_in) out.push_back(intersect(S, E));
            out.push_back(E);
        } else if (S_in) {
            out.push_back(intersect(S, E));
        }
        S = E; S_in = E_in;
    }
    return out;
}

static std::vector<QPointF> intersectConvexPolygons(const std::vector<QPointF>& p1, const std::vector<QPointF>& p2) {
    std::vector<QPointF> res = p1;
    if (res.empty() || p2.size() < 3) return {};
    for (size_t i = 0; i < p2.size(); ++i) {
        const QPointF& a = p2[i];
        const QPointF& b = p2[(i+1)%p2.size()];
        res = clipWithEdge(res, a, b);
        if (res.empty()) break;
    }
    return res;
}

static QPainterPath pathFromPoly(const std::vector<QPointF>& poly) {
    QPainterPath path;
    if (poly.empty()) return path;
    path.moveTo(poly[0]);
    for (size_t i = 1; i < poly.size(); ++i) path.lineTo(poly[i]);
    path.closeSubpath();
    return path;
}

Task07to09Widget::Task07to09Widget(QWidget* parent) : QWidget(parent) {
    auto* root = new QHBoxLayout(this);
    auto* leftPanel = new QWidget(this);
    auto* left = new QVBoxLayout(leftPanel);

    auto* btnClearA = new QPushButton("Clear A", leftPanel);
    auto* btnClearB = new QPushButton("Clear B", leftPanel);
    auto* btnClearAll = new QPushButton("Clear All", leftPanel);

    editSetBox = new QComboBox(leftPanel);
    editSetBox->addItem("Edit: A");
    editSetBox->addItem("Edit: B");

    hullBox = new QCheckBox("Show hulls", leftPanel);
    triBox = new QCheckBox("Show Delaunay", leftPanel);
    labelsBox = new QCheckBox("Labels", leftPanel);
    onlineBox = new QCheckBox("Online", leftPanel);
    hullBox->setChecked(true);

    statusLabel = new QLabel("Left-click to add points. Use combo to choose set.", leftPanel);
    statusLabel->setWordWrap(true);

    // Styling for contrast
    btnClearA->setStyleSheet("QPushButton{background:#b71c1c;color:#fff;padding:6px;border:none;} QPushButton:hover{background:#c62828;}");
    btnClearB->setStyleSheet("QPushButton{background:#0d47a1;color:#fff;padding:6px;border:none;} QPushButton:hover{background:#1565c0;}");
    btnClearAll->setStyleSheet("QPushButton{background:#263238;color:#fff;padding:6px;border:none;} QPushButton:hover{background:#37474f;}");
    leftPanel->setStyleSheet("QCheckBox{color:#111;} QLabel{color:#111;}");

    // Operation selection
    auto* opsBox = new QGroupBox("Fill operation", leftPanel);
    auto* opsLayout = new QVBoxLayout(opsBox);
    opButtons = new QButtonGroup(opsBox); opButtons->setExclusive(true);
    auto* btnInter = new QPushButton("Пересечение", opsBox);
    auto* btnUnion = new QPushButton("Объединение", opsBox);
    auto* btnDiffAB = new QPushButton("A\\B", opsBox);
    auto* btnDiffBA = new QPushButton("B\\A", opsBox);
    for (auto* b : {btnInter, btnUnion, btnDiffAB, btnDiffBA}) { b->setCheckable(true); b->setStyleSheet("QPushButton{background:#fafafa;color:#111;padding:6px;border:1px solid #bbb;} QPushButton:checked{background:#111;color:#fff;border-color:#111;}"); opsLayout->addWidget(b); }
    opButtons->addButton(btnInter, 0);
    opButtons->addButton(btnUnion, 1);
    opButtons->addButton(btnDiffAB, 2);
    opButtons->addButton(btnDiffBA, 3);
    btnInter->setChecked(true);

    left->addWidget(btnClearA);
    left->addWidget(btnClearB);
    left->addWidget(btnClearAll);
    left->addSpacing(8);
    left->addWidget(editSetBox);
    left->addSpacing(8);
    left->addWidget(opsBox);
    left->addSpacing(8);
    left->addWidget(hullBox);
    left->addWidget(triBox);
    left->addWidget(labelsBox);
    left->addWidget(onlineBox);
    left->addSpacing(8);
    left->addWidget(statusLabel);
    left->addStretch(1);

    leftPanel->setFixedWidth(220);

    root->addWidget(leftPanel);
    root->addStretch(1);

    connect(btnClearA, &QPushButton::clicked, this, &Task07to09Widget::clearA);
    connect(btnClearB, &QPushButton::clicked, this, &Task07to09Widget::clearB);
    connect(btnClearAll, &QPushButton::clicked, this, &Task07to09Widget::clearAll);
    connect(hullBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(triBox, &QCheckBox::toggled, this, [this](bool){ recompute(); update(); });
    connect(onlineBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(labelsBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(editSetBox, &QComboBox::currentIndexChanged, this, [this](int idx){ editA = (idx == 0); updateStatus(); });
    connect(opButtons, &QButtonGroup::idToggled, this, [this](int id, bool on){ if (on) { currentOp = static_cast<Op>(id); update(); } });

    setMouseTracking(true);
    recomputeAll();
}

void Task07to09Widget::clearA() {
    pointsA.clear(); hullA.clear(); triA.clear();
    recompute(); updateStatus(); update();
}

void Task07to09Widget::clearB() {
    pointsB.clear(); hullB.clear(); triB.clear();
    recompute(); updateStatus(); update();
}

void Task07to09Widget::clearAll() {
    pointsA.clear(); pointsB.clear(); hullA.clear(); hullB.clear(); triA.clear(); triB.clear();
    recompute(); updateStatus(); update();
}

void Task07to09Widget::recomputeAll() { recompute(); updateStatus(); }

void Task07to09Widget::recompute() {
    hullA = geom::convexHull(pointsA);
    hullB = geom::convexHull(pointsB);
    triA.clear(); triB.clear();
    if (triBox && triBox->isChecked()) {
        triA = geom::delaunayTriangulate(pointsA);
        triB = geom::delaunayTriangulate(pointsB);
    }
}

void Task07to09Widget::updateStatus() {
    QString which = editA ? "A" : "B";
    statusLabel->setText(QString("Editing %1 | A: %2 pts, B: %3 pts | HullA: %4, HullB: %5")
        .arg(which)
        .arg(pointsA.size())
        .arg(pointsB.size())
        .arg(hullA.size())
        .arg(hullB.size()));
}

int Task07to09Widget::findNearestIndex(const std::vector<QPointF>& pts, const QPointF& p, double radius) const {
    double r2 = radius * radius;
    int idx = -1; double best = r2;
    for (int i = 0; i < (int)pts.size(); ++i) {
        double dx = pts[i].x() - p.x();
        double dy = pts[i].y() - p.y();
        double d2 = dx*dx + dy*dy;
        if (d2 <= best) { best = d2; idx = i; }
    }
    return idx;
}

void Task07to09Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    if (e->button() == Qt::LeftButton) {
        if (editA) pointsA.push_back(p); else pointsB.push_back(p);
        if (onlineBox && onlineBox->isChecked()) recompute();
        updateStatus();
        update();
        return;
    }
    if (e->button() == Qt::RightButton) {
        auto& pts = editA ? pointsA : pointsB;
        int idx = findNearestIndex(pts, p, 8.0);
        if (idx >= 0) {
            pts.erase(pts.begin() + idx);
            if (onlineBox && onlineBox->isChecked()) recompute();
            updateStatus();
            update();
        }
        return;
    }
}

void Task07to09Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);

    auto drawPoints = [&](const std::vector<QPointF>& pts, const QColor& color){
        QPen pen(color); pen.setWidth(2); pr.setPen(pen); pr.setBrush(color);
        for (int i = 0; i < (int)pts.size(); ++i) {
            pr.drawEllipse(pts[i], 3.5, 3.5);
            if (labelsBox && labelsBox->isChecked()) {
                pr.setPen(Qt::black);
                pr.drawText(pts[i] + QPointF(6, -6), QString::number(i));
                pr.setPen(pen);
            }
        }
    };

    auto drawHull = [&](const std::vector<QPointF>& hull, const QColor& color){
        if (hull.size() < 2) return;
        QPen pen(color); pen.setWidth(2); pr.setPen(pen);
        for (size_t i = 0; i < hull.size(); ++i) {
            const QPointF& a = hull[i];
            const QPointF& b = hull[(i+1)%hull.size()];
            pr.drawLine(a, b);
        }
    };

    auto drawTriangulation = [&](const std::vector<QPointF>& pts, const std::vector<geom::Triangle>& tris, const QColor& color){
        QPen pen(color.darker()); pen.setWidth(1); pr.setPen(pen);
        for (const auto& t : tris) {
            if (t.a >= 0 && t.b >= 0 && t.c >= 0 && t.a < (int)pts.size() && t.b < (int)pts.size() && t.c < (int)pts.size()) {
                pr.drawLine(pts[t.a], pts[t.b]);
                pr.drawLine(pts[t.b], pts[t.c]);
                pr.drawLine(pts[t.c], pts[t.a]);
            }
        }
    };

    // Build paths for hulls
    QPainterPath pathA = pathFromPoly(hullA);
    QPainterPath pathB = pathFromPoly(hullB);
    auto interPoly = (hullA.size()>=3 && hullB.size()>=3) ? intersectConvexPolygons(hullA, hullB) : std::vector<QPointF>();
    QPainterPath pathInter = pathFromPoly(interPoly);

    // Fill based on operation
    if (currentOp == Op::Intersection) {
        if (!pathInter.isEmpty()) { pr.setPen(Qt::NoPen); pr.setBrush(QColor(150, 0, 150, 90)); pr.drawPath(pathInter); }
    } else if (currentOp == Op::Union) {
        pr.setPen(Qt::NoPen);
        pr.setBrush(QColor(220, 0, 0, 70)); pr.drawPath(pathA);
        pr.setBrush(QColor(0, 120, 220, 70)); pr.drawPath(pathB);
    } else if (currentOp == Op::DiffAB) {
        pr.setPen(Qt::NoPen);
        pr.setBrush(QColor(220, 0, 0, 90)); pr.drawPath(pathA);
        if (!pathInter.isEmpty()) { pr.setBrush(Qt::white); pr.drawPath(pathInter); }
    } else if (currentOp == Op::DiffBA) {
        pr.setPen(Qt::NoPen);
        pr.setBrush(QColor(0, 120, 220, 90)); pr.drawPath(pathB);
        if (!pathInter.isEmpty()) { pr.setBrush(Qt::white); pr.drawPath(pathInter); }
    }

    // Draw points and overlays
    drawPoints(pointsA, QColor(220,0,0));
    drawPoints(pointsB, QColor(0,120,220));

    if (hullBox && hullBox->isChecked()) {
        drawHull(hullA, QColor(180,0,0));
        drawHull(hullB, QColor(0,90,180));
    }

    if (triBox && triBox->isChecked()) {
        drawTriangulation(pointsA, triA, QColor(220,0,0));
        drawTriangulation(pointsB, triB, QColor(0,120,220));
    }
}



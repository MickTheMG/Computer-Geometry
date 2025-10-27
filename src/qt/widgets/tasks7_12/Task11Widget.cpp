#include "Task11Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task11Widget::Task11Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);

    auto* btnClearPoly = new QPushButton("Clear poly", header);
    auto* btnClearPoint = new QPushButton("Clear point", header);
    auto* btnClearAll = new QPushButton("Clear all", header);

    closeBox = new QCheckBox("Close polygon", header);
    fillBox = new QCheckBox("Fill", header);
    labelsBox = new QCheckBox("Labels", header);
    hullBox = new QCheckBox("Show hull", header);
    onlineBox = new QCheckBox("Online", header);

    statusLabel = new QLabel("Left-click: add vertex. Shift+Left: set test point. Right-click: remove vertex.", header);
    resultLabel = new QLabel("Result: n/a", header);

    headerLayout->addWidget(btnClearPoly);
    headerLayout->addWidget(btnClearPoint);
    headerLayout->addWidget(btnClearAll);
    headerLayout->addWidget(closeBox);
    headerLayout->addWidget(fillBox);
    headerLayout->addWidget(labelsBox);
    headerLayout->addWidget(hullBox);
    headerLayout->addWidget(onlineBox);
    headerLayout->addWidget(statusLabel, 1);
    headerLayout->addWidget(resultLabel);
    layout->addWidget(header);

    bottomNotice = new QLabel("", this);
    bottomNotice->setStyleSheet("QLabel { border: 1px solid #c33; padding: 6px; color: #900; background: #ffecec; } ");
    bottomNotice->setVisible(false);
    layout->addWidget(bottomNotice);

    connect(btnClearPoly, &QPushButton::clicked, this, &Task11Widget::clearPoly);
    connect(btnClearPoint, &QPushButton::clicked, this, &Task11Widget::clearPoint);
    connect(btnClearAll, &QPushButton::clicked, this, &Task11Widget::clearAll);

    connect(closeBox, &QCheckBox::toggled, this, [this](bool){ update(); updateBottomNotice(); });
    connect(fillBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(labelsBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(hullBox, &QCheckBox::toggled, this, [this](bool){ recompute(); update(); });
    connect(onlineBox, &QCheckBox::toggled, this, [this](bool){ update(); updateBottomNotice(); });

    setMouseTracking(true);
    recomputeAll();
}

void Task11Widget::clearPoly() { poly.clear(); hull.clear(); recompute(); updateStatus(); updateBottomNotice(); update(); }
void Task11Widget::clearPoint() { testPoint.reset(); updateStatus(); updateBottomNotice(); update(); }
void Task11Widget::clearAll() { poly.clear(); hull.clear(); testPoint.reset(); recompute(); updateStatus(); updateBottomNotice(); update(); }

void Task11Widget::recomputeAll() { recompute(); updateStatus(); updateBottomNotice(); }

bool Task11Widget::checkConvexAndOrientation() {
    isConvex = false; isCCW = false;
    if (poly.size() < 3) return false;
    int n = (int)poly.size();
    int sign = 0;
    for (int i = 0; i < n; ++i) {
        const QPointF& a = poly[i];
        const QPointF& b = poly[(i+1)%n];
        const QPointF& c = poly[(i+2)%n];
        double cr = geom::cross(a, b, c);
        if (std::fabs(cr) < 1e-9) continue;
        int s = cr > 0 ? 1 : -1;
        if (sign == 0) sign = s; else if (sign != s) { isConvex = false; isCCW = false; return false; }
    }
    isConvex = true; isCCW = (sign >= 0);
    return true;
}

void Task11Widget::recompute() {
    hull = geom::convexHull(poly);
    checkConvexAndOrientation();
    if (testPoint && onlineBox && onlineBox->isChecked()) classify();
}

void Task11Widget::updateStatus() {
    QString cls = "n/a";
    int c = classify();
    if (testPoint && poly.size() >= 3) {
        if (c > 0) cls = "inside"; else if (c == 0) cls = "on boundary"; else cls = "outside";
    }
    QString conv = isConvex ? (isCCW ? "convex CCW" : "convex CW") : "not convex";
    statusLabel->setText(QString("Vertices: %1 | Hull: %2 | Poly: %3 | Test: %4")
        .arg(poly.size())
        .arg(hull.size())
        .arg(conv)
        .arg(testPoint ? "set" : "not set"));
    resultLabel->setText(QString("Result: %1").arg(cls));
}

static QPointF clamp01Point(double t) { if (t < 0) t = 0; else if (t > 1) t = 1; return QPointF(t, 0); }

double Task11Widget::distancePointToSegmentSquared(const QPointF& a, const QPointF& b, const QPointF& p) {
    double vx = b.x() - a.x(); double vy = b.y() - a.y();
    double wx = p.x() - a.x(); double wy = p.y() - a.y();
    double vv = vx*vx + vy*vy; if (vv <= 1e-30) { double dx = p.x()-a.x(); double dy = p.y()-a.y(); return dx*dx + dy*dy; }
    double t = (wx*vx + wy*vy) / vv; if (t < 0) t = 0; else if (t > 1) t = 1;
    double px = a.x() + t*vx; double py = a.y() + t*vy;
    double dx = p.x() - px; double dy = p.y() - py;
    return dx*dx + dy*dy;
}

void Task11Widget::updateBottomNotice() {
    if (!testPoint || hull.size() < 3) { bottomNotice->setVisible(false); return; }
    double thresh = 36.0; 
    double best = 1e300;
    for (size_t i = 0; i < hull.size(); ++i) {
        const QPointF& a = hull[i];
        const QPointF& b = hull[(i+1)%hull.size()];
        double d2 = distancePointToSegmentSquared(a, b, *testPoint);
        if (d2 < best) best = d2;
    }
    if (best <= thresh) {
        bottomNotice->setText("Точка у границы красной оболочки");
        bottomNotice->setVisible(true);
    } else {
        bottomNotice->setVisible(false);
    }
}

int Task11Widget::findNearestIndex(const std::vector<QPointF>& pts, const QPointF& p, double radius) const {
    double r2 = radius * radius; int idx = -1; double best = r2;
    for (int i = 0; i < (int)pts.size(); ++i) {
        double dx = pts[i].x() - p.x(); double dy = pts[i].y() - p.y(); double d2 = dx*dx + dy*dy;
        if (d2 <= best) { best = d2; idx = i; }
    }
    return idx;
}

int Task11Widget::classify() const {
    if (!testPoint || poly.size() < 3) return -2;
    return geom::pointInConvexPolygon(poly, *testPoint, 1e-6);
}

void Task11Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    if (e->button() == Qt::LeftButton && (e->modifiers() & Qt::ShiftModifier)) {
        testPoint = p;
        if (onlineBox && onlineBox->isChecked()) updateBottomNotice();
        updateStatus(); update();
        return;
    }
    if (e->button() == Qt::LeftButton) {
        poly.push_back(p);
        if (onlineBox && onlineBox->isChecked()) { recompute(); updateBottomNotice(); }
        updateStatus(); update();
        return;
    }
    if (e->button() == Qt::RightButton) {
        int idx = findNearestIndex(poly, p, 8.0);
        if (idx >= 0) {
            poly.erase(poly.begin() + idx);
            if (onlineBox && onlineBox->isChecked()) { recompute(); updateBottomNotice(); }
            updateStatus(); update();
        }
        return;
    }
}

void Task11Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::black); pen.setWidth(2); pr.setPen(pen);

    if (fillBox && fillBox->isChecked() && poly.size() >= 3) {
        QBrush br(QColor(220, 255, 220, 140)); pr.setBrush(br);
    } else {
        pr.setBrush(Qt::NoBrush);
    }

    for (size_t i = 1; i < poly.size(); ++i) pr.drawLine(poly[i-1], poly[i]);
    if (closeBox && closeBox->isChecked() && poly.size() >= 2) pr.drawLine(poly.back(), poly.front());

    pr.setBrush(QColor(0,120,220));
    for (int i = 0; i < (int)poly.size(); ++i) {
        pr.drawEllipse(poly[i], 3.5, 3.5);
        if (labelsBox && labelsBox->isChecked()) { pr.setPen(Qt::black); pr.drawText(poly[i] + QPointF(6, -6), QString::number(i)); pr.setPen(pen); }
    }

    if (hullBox && hullBox->isChecked()) {
        QPen hp(QColor(200,0,0)); hp.setWidth(2); pr.setPen(hp);
        for (size_t i = 0; i < hull.size(); ++i) { const QPointF& a = hull[i]; const QPointF& b = hull[(i+1)%hull.size()]; pr.drawLine(a, b); }
        pr.setPen(pen);
    }

    if (testPoint) {
        int cls = classify(); QColor c = Qt::gray;
        if (cls > 0) c = Qt::darkGreen; else if (cls == 0) c = Qt::darkYellow; else if (cls < 0) c = Qt::darkRed;
        pr.setBrush(c); pr.drawEllipse(*testPoint, 5, 5);
        if (labelsBox && labelsBox->isChecked()) { pr.setPen(Qt::black); QString txt = QString("(%1, %2)").arg(testPoint->x(), 0, 'f', 2).arg(testPoint->y(), 0, 'f', 2); pr.drawText(*testPoint + QPointF(8, -8), txt); pr.setPen(pen); }
    }
}



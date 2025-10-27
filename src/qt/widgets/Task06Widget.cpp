#include "Task06Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task06Widget::Task06Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);
    auto* btn = new QPushButton("Clear", header);
    triangulateBtn = new QPushButton("Build Delaunay", header);
    statusLabel = new QLabel("Click to add points, then press Build Delaunay.", header);
    statusLabel->setStyleSheet("color: black;");
    headerLayout->addWidget(btn);
    headerLayout->addWidget(triangulateBtn);
    headerLayout->addWidget(statusLabel, 1);
    layout->addWidget(header);
    connect(btn, &QPushButton::clicked, this, &Task06Widget::clearAll);
    connect(triangulateBtn, &QPushButton::clicked, this, &Task06Widget::triangulateOnce);
    setMouseTracking(true);
}

void Task06Widget::clearAll() { points.clear(); tris.clear(); draggingIndex = -1; update(); }

void Task06Widget::triangulateOnce() { recompute(); update(); }

void Task06Widget::recompute() { tris = geom::delaunayTriangulate(points); statusLabel->setText(QString("Points: %1, Tris: %2").arg(points.size()).arg(tris.size())); }

int Task06Widget::pickPoint(const QPointF& p) const {
    const double r = 8.0;
    for (int i = 0; i < (int)points.size(); ++i) if (std::hypot(p.x()-points[i].x(), p.y()-points[i].y()) <= r) return i; return -1;
}

void Task06Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    int idx = pickPoint(p);
    if (idx >= 0) draggingIndex = idx; else { points.push_back(p); recompute(); update(); }
}

void Task06Widget::mouseMoveEvent(QMouseEvent* e) { if (draggingIndex >= 0) { points[draggingIndex] = e->position(); update(); } }

void Task06Widget::mouseReleaseEvent(QMouseEvent* e) { Q_UNUSED(e); if (draggingIndex >= 0) { draggingIndex = -1; recompute(); update(); } }

void Task06Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);
    QPen triPen(QColor(220,30,30)); triPen.setWidth(3); pr.setPen(triPen);
    for (const auto& t : tris) {
        if (t.a < (int)points.size() && t.b < (int)points.size() && t.c < (int)points.size()) {
            pr.drawLine(points[t.a], points[t.b]);
            pr.drawLine(points[t.b], points[t.c]);
            pr.drawLine(points[t.c], points[t.a]);
        }
    }
    pr.setPen(QPen(Qt::black)); pr.setBrush(QColor(0,120,220));
    for (const auto& p : points) pr.drawEllipse(p, 4, 4);
}



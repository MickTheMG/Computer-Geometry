#include "Task05Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task05Widget::Task05Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);
    auto* btn = new QPushButton("Clear", header);
    onlineBox = new QCheckBox("Online", header);
    statusLabel = new QLabel("Click to add points. Press Online to drag.", header);
    headerLayout->addWidget(btn);
    headerLayout->addWidget(onlineBox);
    headerLayout->addWidget(statusLabel, 1);
    layout->addWidget(header);
    connect(btn, &QPushButton::clicked, this, &Task05Widget::clearAll);
    connect(onlineBox, &QCheckBox::toggled, this, &Task05Widget::toggleOnline);
    setMouseTracking(true);
}

void Task05Widget::clearAll() {
    points.clear(); hull.clear(); draggingIndex = -1; recompute(); update();
}

void Task05Widget::toggleOnline(bool on) {
    online = on; if (online) recompute(); update();
}

void Task05Widget::recompute() {
    hull = geom::convexHull(points);
    statusLabel->setText(QString("Points: %1, Hull: %2").arg(points.size()).arg(hull.size()));
}

int Task05Widget::pickPoint(const QPointF& p) const {
    const double r = 8.0;
    for (int i = 0; i < (int)points.size(); ++i) {
        if (std::hypot(p.x()-points[i].x(), p.y()-points[i].y()) <= r) return i;
    }
    return -1;
}

void Task05Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    int idx = pickPoint(p);
    if (idx >= 0) { draggingIndex = idx; }
    else { points.push_back(p); recompute(); update(); }
}

void Task05Widget::mouseMoveEvent(QMouseEvent* e) {
    if (draggingIndex >= 0) {
        points[draggingIndex] = e->position();
        if (online) recompute();
        update();
    }
}

void Task05Widget::mouseReleaseEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    if (draggingIndex >= 0) { draggingIndex = -1; recompute(); update(); }
}

void Task05Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::black); pen.setWidth(2); pr.setPen(pen);
    pr.setBrush(QColor(0, 120, 220, 180));
    for (const auto& p : points) pr.drawEllipse(p, 4, 4);
    if (hull.size() >= 2) {
        QPen hp(QColor(0,160,0)); hp.setWidth(3); pr.setPen(hp);
        for (size_t i = 0; i < hull.size(); ++i) {
            pr.drawLine(hull[i], hull[(i+1)%hull.size()]);
        }
    }
}



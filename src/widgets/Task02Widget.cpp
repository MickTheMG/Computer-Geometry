#include "Task02Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task02Widget::Task02Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);
    auto* btn = new QPushButton("Clear", header);
    onlineBox = new QCheckBox("Online", header);
    resultLabel = new QLabel("Click to set 4 endpoints (two segments).", header);
    headerLayout->addWidget(btn);
    headerLayout->addWidget(onlineBox);
    headerLayout->addWidget(resultLabel, 1);
    layout->addWidget(header);
    connect(btn, &QPushButton::clicked, this, &Task02Widget::clearAll);
    connect(onlineBox, &QCheckBox::toggled, this, [this](bool on){ online = on; updateResult(); update(); });
    setMouseTracking(true);
}

void Task02Widget::clearAll() {
    pointsSet = 0; draggingIndex = -1; online = onlineBox && onlineBox->isChecked();
    resultLabel->setText("Click to set 4 endpoints (two segments).");
    update();
}

QPointF* Task02Widget::indexToPoint(int idx) {
    switch (idx) {
        case 0: return &s1.a;
        case 1: return &s1.b;
        case 2: return &s2.a;
        case 3: return &s2.b;
    }
    return nullptr;
}

int Task02Widget::pickHandle(const QPointF& p) const {
    const double r = 8.0;
    auto within = [&](const QPointF& q){ return std::hypot(p.x()-q.x(), p.y()-q.y()) <= r; };
    if (pointsSet >= 1 && within(s1.a)) return 0;
    if (pointsSet >= 2 && within(s1.b)) return 1;
    if (pointsSet >= 3 && within(s2.a)) return 2;
    if (pointsSet >= 4 && within(s2.b)) return 3;
    return -1;
}

void Task02Widget::updateResult() {
    if (pointsSet < 4) { resultLabel->setText("Click to set 4 endpoints (two segments)."); return; }
    auto r = geom::intersectSegments(s1, s2);
    if (r.type == geom::Intersection::Type::None) resultLabel->setText("No intersection");
    else if (r.type == geom::Intersection::Type::Point) resultLabel->setText(QString("Point: (%1, %2)").arg(r.p1.x()).arg(r.p1.y()));
    else resultLabel->setText(QString("Overlap: (%1, %2) - (%3, %4)").arg(r.p1.x()).arg(r.p1.y()).arg(r.p2.x()).arg(r.p2.y()));
}

void Task02Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    if (pointsSet < 4) {
        if (pointsSet == 0) s1.a = p; else if (pointsSet == 1) s1.b = p; else if (pointsSet == 2) s2.a = p; else s2.b = p;
        ++pointsSet; updateResult(); update();
    } else {
        draggingIndex = pickHandle(p);
    }
}

void Task02Widget::mouseMoveEvent(QMouseEvent* e) {
    if (draggingIndex >= 0) {
        QPointF* pt = indexToPoint(draggingIndex);
        if (pt) *pt = e->position();
        if (online) updateResult();
        update();
    }
}

void Task02Widget::mouseReleaseEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    if (draggingIndex >= 0) { draggingIndex = -1; updateResult(); update(); }
}

void Task02Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::black); pen.setWidth(2); pr.setPen(pen);
    if (pointsSet >= 2) pr.drawLine(s1.a, s1.b);
    if (pointsSet >= 4) pr.drawLine(s2.a, s2.b);
    pr.setBrush(QColor(220,0,0));
    if (pointsSet >= 1) pr.drawEllipse(s1.a, 4, 4);
    if (pointsSet >= 2) pr.drawEllipse(s1.b, 4, 4);
    pr.setBrush(QColor(0,120,220));
    if (pointsSet >= 3) pr.drawEllipse(s2.a, 4, 4);
    if (pointsSet >= 4) pr.drawEllipse(s2.b, 4, 4);

    if (pointsSet >= 4) {
        auto r = geom::intersectSegments(s1, s2);
        if (r.type == geom::Intersection::Type::Point) {
            pr.setBrush(Qt::green);
            pr.drawEllipse(r.p1, 5, 5);
            if (online) {
                
                QPen tp(Qt::darkGreen); pr.setPen(tp);
                QString txt = QString("(%1, %2)")
                    .arg(r.p1.x(), 0, 'f', 2)
                    .arg(r.p1.y(), 0, 'f', 2);
                pr.drawText(r.p1 + QPointF(8, -8), txt);
            }
        } else if (r.type == geom::Intersection::Type::Segment) {
            QPen ip(Qt::green); ip.setWidth(4); pr.setPen(ip);
            pr.drawLine(r.p1, r.p2);
        }
    }
}


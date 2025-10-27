#include "Task01Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task01Widget::Task01Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);
    auto* btn = new QPushButton("Clear", header);
    resultLabel = new QLabel("Click to create segment endpoints (2) and a point (1).", header);
    lcd = new QLCDNumber(header); lcd->setDigitCount(2); lcd->display(0);
    headerLayout->addWidget(btn);
    headerLayout->addWidget(resultLabel, 1);
    headerLayout->addWidget(lcd);
    layout->addWidget(header);
    connect(btn, &QPushButton::clicked, this, &Task01Widget::clearAll);
    setMouseTracking(true);
}

void Task01Widget::clearAll() {
    segA.reset(); segB.reset(); pointP.reset();
    resultLabel->setText("Click to create segment endpoints (2) and a point (1).");
    lcd->display(0);
    update();
}

int Task01Widget::computeResult() const {
    if (!(segA && segB && pointP)) return 0;
    
    double len = std::hypot(segB->x() - segA->x(), segB->y() - segA->y());
    double pixelTol = 3.0; 
    double epsArea = len * pixelTol; 
    int r = geom::pointRelativeToSegment(*segA, *segB, *pointP, epsArea);
    return r;
}

void Task01Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    if (!segA) segA = p;
    else if (!segB) segB = p;
    else if (!pointP) pointP = p;
    else draggingIndex = pickHandle(p);

    if (segA && segB && pointP) {
        int r = computeResult();
        lcd->display(r);
        if (r > 0) resultLabel->setText("Result: 1 (point is left)");
        else if (r < 0) resultLabel->setText("Result: -1 (point is right)");
        else resultLabel->setText("Result: 0 (point on segment)");
    }
    update();
}

int Task01Widget::pickHandle(const QPointF& p) const {
    const double r = 8.0;
    auto hit = [&](const std::optional<QPointF>& q){ return q && std::hypot(p.x()-q->x(), p.y()-q->y()) <= r; };
    if (hit(segA)) return 0; if (hit(segB)) return 1; if (hit(pointP)) return 2; return -1;
}

void Task01Widget::mouseMoveEvent(QMouseEvent* e) {
    if (draggingIndex >= 0) {
        if (draggingIndex == 0) segA = e->position();
        else if (draggingIndex == 1) segB = e->position();
        else if (draggingIndex == 2) pointP = e->position();
        if (segA && segB && pointP) lcd->display(computeResult());
        update();
    }
}

void Task01Widget::mouseReleaseEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    if (draggingIndex >= 0) { draggingIndex = -1; if (segA && segB && pointP) lcd->display(computeResult()); update(); }
}

void Task01Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::black); pen.setWidth(2); pr.setPen(pen);
    if (segA && segB) {
        pr.drawLine(*segA, *segB);
        pr.setBrush(Qt::red);
        pr.drawEllipse(*segA, 4, 4);
        pr.drawEllipse(*segB, 4, 4);
    }
    if (pointP) {
        pr.setBrush(Qt::blue);
        pr.drawEllipse(*pointP, 4, 4);
    }
}



#include "Task13Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task13Widget::Task13Widget(QWidget* parent) : QWidget(parent) {
    auto* root = new QHBoxLayout(this);

    // Left controls panel (high-contrast like tasks 7–9)
    auto* leftPanel = new QWidget(this);
    auto* left = new QVBoxLayout(leftPanel);

    auto* btnClear = new QPushButton("Clear", leftPanel);
    labelsBox = new QCheckBox("Labels", leftPanel);
    onlineBox = new QCheckBox("Online", leftPanel);

    statusLabel = new QLabel("ЛКМ: добавить до 4 точек. Перетаскивание: двигать точки.", leftPanel);
    statusLabel->setWordWrap(true);

    // Styling
    btnClear->setStyleSheet("QPushButton{background:#263238;color:#fff;padding:6px;border:none;} QPushButton:hover{background:#37474f;}");
    leftPanel->setStyleSheet("QCheckBox{color:#111;} QLabel{color:#111;}");

    left->addWidget(btnClear);
    left->addSpacing(8);
    left->addWidget(labelsBox);
    left->addWidget(onlineBox);
    left->addSpacing(8);
    left->addWidget(statusLabel);
    left->addStretch(1);

    leftPanel->setFixedWidth(220);

    root->addWidget(leftPanel);
    root->addStretch(1);

    connect(btnClear, &QPushButton::clicked, this, &Task13Widget::clearAll);

    setMouseTracking(true);
    updateStatus();
}

void Task13Widget::clearAll() {
    controlPoints.clear(); draggingIndex = -1;
    updateStatus(); update();
}

void Task13Widget::updateStatus() {
    QString ready = (controlPoints.size() == 4) ? "ready" : "need 4 points";
    statusLabel->setText(QString("Points: %1 (%2)").arg(controlPoints.size()).arg(ready));
}

int Task13Widget::pickHandle(const QPointF& p) const {
    const double r = 8.0; double r2 = r*r; int idx = -1; double best = r2;
    for (int i = 0; i < (int)controlPoints.size(); ++i) {
        double dx = p.x() - controlPoints[i].x(); double dy = p.y() - controlPoints[i].y();
        double d2 = dx*dx + dy*dy; if (d2 <= best) { best = d2; idx = i; }
    }
    return idx;
}

void Task13Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    if (e->button() == Qt::LeftButton) {
        if ((int)controlPoints.size() < 4) {
            controlPoints.push_back(p);
            updateStatus(); update();
        } else {
            int idx = pickHandle(p);
            if (idx >= 0) draggingIndex = idx;
        }
    }
}

void Task13Widget::mouseMoveEvent(QMouseEvent* e) {
    if (draggingIndex >= 0) {
        controlPoints[draggingIndex] = e->position();
        if (onlineBox && onlineBox->isChecked()) update();
    }
}

void Task13Widget::mouseReleaseEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    if (draggingIndex >= 0) { draggingIndex = -1; update(); }
}

void Task13Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);

    // Draw control polygon
    QPen polyPen(QColor(120,120,120)); polyPen.setWidth(1); pr.setPen(polyPen);
    pr.setBrush(Qt::NoBrush);
    for (size_t i = 1; i < controlPoints.size(); ++i) pr.drawLine(controlPoints[i-1], controlPoints[i]);

    // Draw control points
    for (int i = 0; i < (int)controlPoints.size(); ++i) {
        pr.setBrush(QColor(220,0,0));
        QPen p(Qt::black); p.setWidth(1); pr.setPen(p);
        pr.drawEllipse(controlPoints[i], 4.5, 4.5);
        if (labelsBox && labelsBox->isChecked()) {
            pr.setPen(Qt::black);
            pr.drawText(controlPoints[i] + QPointF(6, -6), QString("P%1").arg(i));
        }
    }

    // Draw cubic Bezier if 4 points ready
    if (controlPoints.size() == 4) {
        QPainterPath path; path.moveTo(controlPoints[0]);
        path.cubicTo(controlPoints[1], controlPoints[2], controlPoints[3]);
        QPen curvePen(QColor(85, 26, 139)); curvePen.setWidth(3);
        pr.setPen(curvePen); pr.setBrush(Qt::NoBrush);
        pr.drawPath(path);
    }
}

#include "Task10Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task10Widget::Task10Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);

    auto* btnClear = new QPushButton("Clear", header);
    closeBox = new QCheckBox("Close polygon", header);
    fillBox = new QCheckBox("Fill", header);
    labelsBox = new QCheckBox("Labels", header);
    hullBox = new QCheckBox("Show hull", header);
    onlineBox = new QCheckBox("Online", header);

    statusLabel = new QLabel("Left-click to add vertices. Right-click to remove nearest.", header);

    headerLayout->addWidget(btnClear);
    headerLayout->addWidget(closeBox);
    headerLayout->addWidget(fillBox);
    headerLayout->addWidget(labelsBox);
    headerLayout->addWidget(hullBox);
    headerLayout->addWidget(onlineBox);
    headerLayout->addWidget(statusLabel, 1);
    layout->addWidget(header);

    connect(btnClear, &QPushButton::clicked, this, &Task10Widget::clearAll);
    connect(closeBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(fillBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(labelsBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(hullBox, &QCheckBox::toggled, this, [this](bool){ recompute(); update(); });
    connect(onlineBox, &QCheckBox::toggled, this, [this](bool){ update(); });

    setMouseTracking(true);
    recomputeAll();
}

void Task10Widget::clearAll() {
    vertices.clear(); hull.clear();
    recompute(); updateStatus(); update();
}

void Task10Widget::recomputeAll() { recompute(); updateStatus(); }

void Task10Widget::recompute() {
    hull = geom::convexHull(vertices);
}

void Task10Widget::updateStatus() {
    statusLabel->setText(QString("Vertices: %1 | Hull: %2")
        .arg(vertices.size())
        .arg(hull.size()));
}

int Task10Widget::findNearestIndex(const std::vector<QPointF>& pts, const QPointF& p, double radius) const {
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

void Task10Widget::mousePressEvent(QMouseEvent* e) {
    QPointF p = e->position();
    if (e->button() == Qt::LeftButton) {
        vertices.push_back(p);
        if (onlineBox && onlineBox->isChecked()) recompute();
        updateStatus();
        update();
        return;
    }
    if (e->button() == Qt::RightButton) {
        int idx = findNearestIndex(vertices, p, 8.0);
        if (idx >= 0) {
            vertices.erase(vertices.begin() + idx);
            if (onlineBox && onlineBox->isChecked()) recompute();
            updateStatus();
            update();
        }
        return;
    }
}

void Task10Widget::paintEvent(QPaintEvent*) {
    QPainter pr(this);
    pr.fillRect(rect(), Qt::white);
    pr.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::black); pen.setWidth(2); pr.setPen(pen);

    if (!vertices.empty()) {
        if (fillBox && fillBox->isChecked() && vertices.size() >= 3) {
            QBrush br(QColor(255, 220, 180, 120)); pr.setBrush(br);
        } else {
            pr.setBrush(Qt::NoBrush);
        }

        for (size_t i = 1; i < vertices.size(); ++i) {
            pr.drawLine(vertices[i-1], vertices[i]);
        }
        if (closeBox && closeBox->isChecked() && vertices.size() >= 2) {
            pr.drawLine(vertices.back(), vertices.front());
        }
    }

    pr.setBrush(QColor(0,120,220));
    for (int i = 0; i < (int)vertices.size(); ++i) {
        pr.drawEllipse(vertices[i], 3.5, 3.5);
        if (labelsBox && labelsBox->isChecked()) {
            pr.setPen(Qt::black);
            pr.drawText(vertices[i] + QPointF(6, -6), QString::number(i));
            pr.setPen(pen);
        }
    }

    if (hullBox && hullBox->isChecked()) {
        QPen hp(QColor(200,0,0)); hp.setWidth(2); pr.setPen(hp);
        for (size_t i = 0; i < hull.size(); ++i) {
            const QPointF& a = hull[i];
            const QPointF& b = hull[(i+1)%hull.size()];
            pr.drawLine(a, b);
        }
    }
}



#include "Task07to09Widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

Task07to09Widget::Task07to09Widget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* header = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(header);

    auto* btnClearA = new QPushButton("Clear A", header);
    auto* btnClearB = new QPushButton("Clear B", header);
    auto* btnClearAll = new QPushButton("Clear All", header);

    editSetBox = new QComboBox(header);
    editSetBox->addItem("Edit: A");
    editSetBox->addItem("Edit: B");

    hullBox = new QCheckBox("Show hulls", header);
    triBox = new QCheckBox("Show Delaunay", header);
    labelsBox = new QCheckBox("Labels", header);
    onlineBox = new QCheckBox("Online", header);
    hullBox->setChecked(true);

    statusLabel = new QLabel("Left-click to add points. Use combo to choose set.", header);

    headerLayout->addWidget(btnClearA);
    headerLayout->addWidget(btnClearB);
    headerLayout->addWidget(btnClearAll);
    headerLayout->addWidget(editSetBox);
    headerLayout->addWidget(hullBox);
    headerLayout->addWidget(triBox);
    headerLayout->addWidget(labelsBox);
    headerLayout->addWidget(onlineBox);
    headerLayout->addWidget(statusLabel, 1);
    layout->addWidget(header);

    connect(btnClearA, &QPushButton::clicked, this, &Task07to09Widget::clearA);
    connect(btnClearB, &QPushButton::clicked, this, &Task07to09Widget::clearB);
    connect(btnClearAll, &QPushButton::clicked, this, &Task07to09Widget::clearAll);
    connect(hullBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(triBox, &QCheckBox::toggled, this, [this](bool){ recompute(); update(); });
    connect(onlineBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(labelsBox, &QCheckBox::toggled, this, [this](bool){ update(); });
    connect(editSetBox, &QComboBox::currentIndexChanged, this, [this](int idx){ editA = (idx == 0); updateStatus(); });

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



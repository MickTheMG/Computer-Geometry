#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "../common/Geometry.h"

class Task06Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task06Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearAll();
    void triangulateOnce();

private:
    std::vector<QPointF> points;
    std::vector<geom::Triangle> tris;
    int draggingIndex = -1;
    QLabel* statusLabel = nullptr;
    QPushButton* triangulateBtn = nullptr;
    void recompute();
    int pickPoint(const QPointF& p) const;
};


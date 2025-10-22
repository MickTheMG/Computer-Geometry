#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "../common/Geometry.h"

class Task02Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task02Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearAll();

private:
    geom::Segment s1{{}, {}};
    geom::Segment s2{{}, {}};
    int pointsSet = 0;
    int draggingIndex = -1; 
    bool online = false;
    QLabel* resultLabel = nullptr;
    QCheckBox* onlineBox = nullptr;

    QPointF* indexToPoint(int idx);
    void updateResult();
    int pickHandle(const QPointF& p) const;
};


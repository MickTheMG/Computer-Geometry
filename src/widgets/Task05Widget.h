#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "../common/Geometry.h"

class Task05Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task05Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearAll();
    void toggleOnline(bool on);

private:
    std::vector<QPointF> points;
    std::vector<QPointF> hull;
    int draggingIndex = -1;
    bool online = false;
    QLabel* statusLabel = nullptr;
    QCheckBox* onlineBox = nullptr;
    void recompute();
    int pickPoint(const QPointF& p) const;
};


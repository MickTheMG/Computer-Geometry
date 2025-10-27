#pragma once
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <vector>
#include <optional>
#include "../../../cpp/common/Geometry.h"

class Task10Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task10Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearAll();
    void recomputeAll();

private:
    std::vector<QPointF> vertices;
    std::vector<QPointF> hull;

    QLabel* statusLabel = nullptr;
    QCheckBox* closeBox = nullptr;
    QCheckBox* fillBox = nullptr;
    QCheckBox* labelsBox = nullptr;
    QCheckBox* hullBox = nullptr;
    QCheckBox* onlineBox = nullptr;

    void recompute();
    void updateStatus();
    int findNearestIndex(const std::vector<QPointF>& pts, const QPointF& p, double radius) const;
};



#pragma once
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <vector>
#include <optional>
#include "../../../cpp/common/Geometry.h"

class Task11Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task11Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearPoly();
    void clearPoint();
    void clearAll();
    void recomputeAll();

private:
    std::vector<QPointF> poly; 
    std::vector<QPointF> hull; 
    std::optional<QPointF> testPoint; 

    QLabel* statusLabel = nullptr;
    QLabel* resultLabel = nullptr;
    QLabel* bottomNotice = nullptr;
    QCheckBox* closeBox = nullptr;
    QCheckBox* fillBox = nullptr;
    QCheckBox* labelsBox = nullptr;
    QCheckBox* hullBox = nullptr;
    QCheckBox* onlineBox = nullptr;

    bool isConvex = false;
    bool isCCW = false;

    void recompute();
    void updateStatus();
    void updateBottomNotice();
    int findNearestIndex(const std::vector<QPointF>& pts, const QPointF& p, double radius) const;
    int classify() const; 
    bool checkConvexAndOrientation();
    static double distancePointToSegmentSquared(const QPointF& a, const QPointF& b, const QPointF& p);
};



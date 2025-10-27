#pragma once
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <vector>
#include <optional>
#include "../../../cpp/common/Geometry.h"

class Task07to09Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task07to09Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearA();
    void clearB();
    void clearAll();
    void recomputeAll();

private:
    std::vector<QPointF> pointsA;
    std::vector<QPointF> pointsB;
    std::vector<QPointF> hullA;
    std::vector<QPointF> hullB;
    std::vector<geom::Triangle> triA;
    std::vector<geom::Triangle> triB;

    bool editA = true; 

    QLabel* statusLabel = nullptr;
    QCheckBox* hullBox = nullptr;
    QCheckBox* triBox = nullptr;
    QCheckBox* labelsBox = nullptr;
    QCheckBox* onlineBox = nullptr;
    QComboBox* editSetBox = nullptr;

    void recompute();
    void updateStatus();
    int findNearestIndex(const std::vector<QPointF>& pts, const QPointF& p, double radius) const;
};



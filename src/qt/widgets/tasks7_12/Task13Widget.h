#pragma once
#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <vector>
#include <optional>

class Task13Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task13Widget(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void clearAll();

private:
    std::vector<QPointF> controlPoints; // up to 4
    int draggingIndex = -1;

    QLabel* statusLabel = nullptr;
    QCheckBox* labelsBox = nullptr;
    QCheckBox* onlineBox = nullptr;

    void updateStatus();
    int pickHandle(const QPointF& p) const;
};

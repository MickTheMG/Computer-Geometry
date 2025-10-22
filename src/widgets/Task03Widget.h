#pragma once
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLCDNumber>
#include "../common/BigDecimal.h"

class Task03Widget : public QWidget {
    Q_OBJECT
public:
    explicit Task03Widget(QWidget* parent = nullptr);

private slots:
    void compute();

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QLineEdit *ax, *ay, *bx, *by, *px, *py;
    QLabel* resultLabel;
    QLCDNumber* lcd;
};


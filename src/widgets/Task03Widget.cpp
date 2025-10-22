#include "Task03Widget.h"
#include <QPainter>

static BigDecimal crossBD(const BigDecimal& ax, const BigDecimal& ay, const BigDecimal& bx, const BigDecimal& by, const BigDecimal& px, const BigDecimal& py) {
    
    BigDecimal bxax = BigDecimal::sub(bx, ax);
    BigDecimal byay = BigDecimal::sub(by, ay);
    BigDecimal pyay = BigDecimal::sub(py, ay);
    BigDecimal pxax = BigDecimal::sub(px, ax);
    BigDecimal m1 = BigDecimal::mul(bxax, pyay);
    BigDecimal m2 = BigDecimal::mul(byay, pxax);
    return BigDecimal::sub(m1, m2);
}

Task03Widget::Task03Widget(QWidget* parent) : QWidget(parent) {
    auto* root = new QVBoxLayout(this);
    auto* row = new QHBoxLayout();
    ax = new QLineEdit("1"); ay = new QLineEdit("1");
    bx = new QLineEdit("2"); by = new QLineEdit("2");
    px = new QLineEdit("1.5"); py = new QLineEdit("1.8");
    row->addWidget(new QLabel("A(x,y):")); row->addWidget(ax); row->addWidget(ay);
    row->addWidget(new QLabel("B(x,y):")); row->addWidget(bx); row->addWidget(by);
    row->addWidget(new QLabel("P(x,y):")); row->addWidget(px); row->addWidget(py);
    auto* btn = new QPushButton("Compute");
    lcd = new QLCDNumber(this); lcd->setDigitCount(2); lcd->display(0);
    row->addWidget(btn);
    root->addLayout(row);
    resultLabel = new QLabel("Enter A(x,y), B(x,y) of segment and P(x,y) of point.");
    auto* row2 = new QHBoxLayout(); row2->addWidget(resultLabel, 1); row2->addWidget(lcd);
    root->addLayout(row2);
    connect(btn, &QPushButton::clicked, this, &Task03Widget::compute);
}

void Task03Widget::compute() {
    BigDecimal AX(ax->text().toStdString());
    BigDecimal AY(ay->text().toStdString());
    BigDecimal BX(bx->text().toStdString());
    BigDecimal BY(by->text().toStdString());
    BigDecimal PX(px->text().toStdString());
    BigDecimal PY(py->text().toStdString());
    BigDecimal c = crossBD(AX, AY, BX, BY, PX, PY);
    int s = c.sign();
    if (s > 0) resultLabel->setText("Result: 1 (left)");
    else if (s < 0) resultLabel->setText("Result: -1 (right)");
    else resultLabel->setText("Result: 0 (collinear or on segment)");
    lcd->display(s);
    update();
}

void Task03Widget::paintEvent(QPaintEvent*) {
    
    QPainter pr(this);
    pr.fillRect(rect(), QColor(250,250,250));
    pr.setPen(QPen(QColor(230,230,230)));
    for (int x = 0; x < width(); x += 25) pr.drawLine(x, 0, x, height());
    for (int y = 0; y < height(); y += 25) pr.drawLine(0, y, width(), y);
}


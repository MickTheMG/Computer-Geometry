#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include "widgets/Task03Widget.h"
#include "widgets/tasks7_12/Task07to09Widget.h"
#include "widgets/tasks7_12/Task10Widget.h"
#include "widgets/tasks7_12/Task11Widget.h"
#include "widgets/tasks7_12/Task12Widget.h"


MainWindow::MainWindow(QWidget* parent, int initialTask) : QMainWindow(parent) {
    auto* tabs = new QTabWidget(this);
    tabs->addTab(new Task03Widget(this), "Task 3: BigDecimal Orientation");
    tabs->addTab(new Task07to09Widget(this), "Tasks 7-9: Convex Ops");
    tabs->addTab(new Task10Widget(this), "Task 10: Arbitrary Ops");
    tabs->addTab(new Task11Widget(this), "Task 11: Point vs Convex");
    tabs->addTab(new Task12Widget(this), "Task 12: Point vs Polygon");
    if (initialTask >= 3 && initialTask <= 12) {
        int mapIdx[13] = {-1, -1, -1, 0, -1, -1, -1, 1, 1, 1, 2, 3, 4};
        int idx = mapIdx[initialTask];
        if (idx >= 0) tabs->setCurrentIndex(idx);
    }
    setCentralWidget(tabs);
    setWindowTitle("Computational Geometry Tasks");
    resize(1000, 700);
}
 


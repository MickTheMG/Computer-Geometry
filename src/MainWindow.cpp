#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include "widgets/Task01Widget.h"
#include "widgets/Task02Widget.h"
#include "widgets/Task03Widget.h"
#include "widgets/Task05Widget.h"
#include "widgets/Task06Widget.h"
#include "widgets/tasks7_12/Task07to09Widget.h"
#include "widgets/tasks7_12/Task10Widget.h"
#include "widgets/tasks7_12/Task11Widget.h"
#include "widgets/tasks7_12/Task12Widget.h"

MainWindow::MainWindow(QWidget* parent, int initialTask) : QMainWindow(parent) {
    auto* tabs = new QTabWidget(this);
    tabs->addTab(new Task01Widget(this), "Task 1: Point vs Segment");
    tabs->addTab(new Task02Widget(this), "Task 2: Segment Intersection");
    tabs->addTab(new Task03Widget(this), "Task 3: Big-Number Orientation");
    tabs->addTab(new Task05Widget(this), "Task 5: Convex Hull");
    tabs->addTab(new Task06Widget(this), "Task 6: Delaunay");
    tabs->addTab(new Task07to09Widget(this), "Tasks 7-9: Convex Ops");
    tabs->addTab(new Task10Widget(this), "Task 10: Arbitrary Ops");
    tabs->addTab(new Task11Widget(this), "Task 11: Point vs Convex");
    tabs->addTab(new Task12Widget(this), "Task 12: Point vs Polygon");
    if (initialTask >= 1 && initialTask <= 12) {
        // map task number to tab index
        int mapIdx[13] = {0, 0, 1, 2, -1, 3, 4, 5, 5, 5, 6, 7, 8};
        int idx = mapIdx[initialTask];
        if (idx >= 0) tabs->setCurrentIndex(idx);
    }
    setCentralWidget(tabs);
    setWindowTitle("Computational Geometry Tasks");
    resize(1000, 700);
}
 

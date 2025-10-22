#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include "widgets/Task01Widget.h"
#include "widgets/Task02Widget.h"
#include "widgets/Task03Widget.h"
#include "widgets/Task04Widget.h"
#include "widgets/Task05Widget.h"
#include "widgets/Task06Widget.h"


MainWindow::MainWindow(QWidget* parent, int initialTask) : QMainWindow(parent) {
    auto* tabs = new QTabWidget(this);
    tabs->addTab(new Task01Widget(this), "Task 1: Point vs Segment");
    tabs->addTab(new Task02Widget(this), "Task 2: Segment Intersection");
    tabs->addTab(new Task03Widget(this), "Task 3: Big-Number Orientation");
    tabs->addTab(new Task04Widget(this), "Task 4");
    tabs->addTab(new Task05Widget(this), "Task 5: Convex Hull");
    tabs->addTab(new Task06Widget(this), "Task 6: Delaunay");
    if (initialTask >= 1 && initialTask <= 6) {
        tabs->setCurrentIndex(initialTask - 1);
    }
    setCentralWidget(tabs);
    setWindowTitle("Computational Geometry Tasks");
    resize(1000, 700);
}
 

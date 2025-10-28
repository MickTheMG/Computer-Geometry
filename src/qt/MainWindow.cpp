#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include "widgets/tasks7_12/Task07to09Widget.h"
#include "widgets/tasks7_12/Task13Widget.h"


MainWindow::MainWindow(QWidget* parent, int initialTask) : QMainWindow(parent) {
    auto* tabs = new QTabWidget(this);
    tabs->addTab(new Task07to09Widget(this), "Tasks 7-9: Convex Ops");
    tabs->addTab(new Task13Widget(this), "Task 13: Cubic Bezier");
    setCentralWidget(tabs);
    setWindowTitle("Computational Geometry Tasks (7-9, 13)");
    resize(1000, 700);
}
 


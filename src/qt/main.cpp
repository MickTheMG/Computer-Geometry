#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    int initialTask = -1;
    if (argc >= 2) {
        // allow: ./KompGeom --task N
        for (int i = 1; i < argc - 1; ++i) if (std::string(argv[i]) == "--task") initialTask = std::atoi(argv[i+1]);
    }
    MainWindow w(nullptr, initialTask);
    w.show();
    return app.exec();
}
 


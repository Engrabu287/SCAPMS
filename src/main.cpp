#include "scapms/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("SCAPMS");
    app.setOrganizationName("COS202 Project");
    scapms::MainWindow window;
    window.show();
    return app.exec();
}

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QStringLiteral("Master Gekus"));
    a.setApplicationName(QStringLiteral("Waterfall"));
    a.setApplicationVersion(QStringLiteral("Version " GIT_DESCRIBE " from "
                                           __DATE__ " " __TIME__));
    MainWindow w;
    w.show();

    return a.exec();
}

#include "mainwindow.h"

#include<QApplication>
#include<QMenu>
#include<QMenuBar>
#include<QAction>
#include<QDir>
#include<QIcon>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QDir folder(QStringLiteral(":/res/mainicon"));
    QIcon icon;
    for(const QString& file_name : folder.entryList())
        icon.addFile(folder.filePath(file_name));
    setWindowIcon(icon);

    setWindowTitle(QStringLiteral("Waterfall"));

    setObjectName(QStringLiteral("MainWindow"));
    setMinimumSize(QSize(640, 480));

    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuGame = new QMenu(QStringLiteral("Игра"), menuBar);
    QAction *actionQuit = new QAction(QStringLiteral("Выход"), this);
    actionQuit->setMenuRole(QAction::QuitRole);
    menuGame->addAction(actionQuit);

    QMenu* menuHelp = new QMenu(QStringLiteral("Помощь"), menuBar);
    QAction *actionAbout = new QAction(QStringLiteral("О программе..."), this);
    actionAbout->setMenuRole(QAction::AboutRole);
    menuHelp->addAction(actionAbout);

    menuBar->addAction(menuGame->menuAction());
    menuBar->addAction(menuHelp->menuAction());
    setMenuBar(menuBar);

    connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
}


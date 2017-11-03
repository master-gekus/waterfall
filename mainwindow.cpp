﻿#include "mainwindow.h"

#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDir>
#include <QIcon>
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QDialogButtonBox>

#include "central_widget.h"
namespace
{
    class AboutBox : public QDialog
    {
    public:
        explicit AboutBox(QWidget *parent = 0) :
            QDialog(parent)
        {
            setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
            QVBoxLayout* layout_main = new QVBoxLayout(this);
            layout_main->setSizeConstraint(QLayout::SetFixedSize);

            QFrame* frame_main = new QFrame(this);
            QHBoxLayout* layout_info = new QHBoxLayout(frame_main);
            layout_info->setContentsMargins(0, 0, 0, 0);

            QFrame* frame_icon = new QFrame(frame_main);
            QVBoxLayout* layout_icon = new QVBoxLayout(frame_icon);
            layout_icon->setContentsMargins(0, 0, 32, 0);

            QLabel* label_icon = new QLabel(frame_icon);
            label_icon->setPixmap(QPixmap(QString::fromUtf8(":/res/mainicon/128x128.png")));
            layout_icon->addWidget(label_icon);
            layout_icon->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

            layout_info->addWidget(frame_icon);

            QFrame*  frame_text = new QFrame(frame_main);
            QVBoxLayout* layout_text = new QVBoxLayout(frame_text);
            layout_text->setContentsMargins(0, 0, 0, 0);
            QLabel* label_appname = new QLabel(QApplication::applicationName(), frame_text);
            label_appname->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 12pt;"));
            label_appname->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

            layout_text->addWidget(label_appname);

            QLabel* label_version = new QLabel(QApplication::applicationVersion(), frame_text);
            label_version->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
            label_version->setWordWrap(true);

            layout_text->addWidget(label_version);

            QLabel* label_4 = new QLabel(frame_text);
            label_4->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
            label_4->setWordWrap(false);

            layout_text->addWidget(label_4);

            layout_text->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

            layout_info->addWidget(frame_text);

            layout_main->addWidget(frame_main);

            QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
            buttonBox->setOrientation(Qt::Horizontal);
            buttonBox->setStandardButtons(QDialogButtonBox::Ok);
            layout_main->addWidget(buttonBox);

            QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        }
    };
}   // namespace

QIcon MainWindow::main_icon_;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QDir folder(QStringLiteral(":/res/mainicon"));
    for(const QString& file_name : folder.entryList())
        main_icon_.addFile(folder.filePath(file_name));
    QApplication::setWindowIcon(main_icon_);

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
    connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(on_action_about()));

    setCentralWidget(new CentralWidget(this));
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_action_about()
{
    AboutBox(this).exec();
}


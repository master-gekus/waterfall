#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QIcon;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    static const QIcon& mainIcon()
    {
        return main_icon_;
    }

private:
    static QIcon main_icon_;
};

#endif // MAINWINDOW_H

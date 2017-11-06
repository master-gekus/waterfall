#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QIcon;
class QAction;
class QActionGroup;
class CentralWidget;

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
    CentralWidget *game_field_;
    QAction **field_size_actions_;
    QActionGroup *field_size_group_;

private slots:
    void on_action_about();
    void on_action_field_size();
    void on_action_game_new();
};

#endif // MAINWINDOW_H

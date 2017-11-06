#ifndef HISCORES_H
#define HISCORES_H

#include <QDialog>

class QComboBox;
class QTreeWidget;

class Hiscores : public QDialog
{
    Q_OBJECT

public:
    explicit Hiscores(QWidget *parent = 0);
    ~Hiscores();

private:
    QComboBox* combo_;
    QTreeWidget* tree_;
};

#endif // HISCORES_H

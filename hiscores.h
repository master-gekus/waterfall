#ifndef HISCORES_H
#define HISCORES_H

#include <QDialog>

class QComboBox;
class QTreeWidget;

class Hiscores : public QDialog
{
    Q_OBJECT

public:
    Hiscores(QWidget *parent, int field_size);
    ~Hiscores();

public:
    static QString timeToString(quint64 time);
    static bool isInHiScore(int field_size, quint64 time, quint32 clicks);
    static void addHiScore(int field_size, const QString& name, quint64 time, quint32 clicks);

private:
    QComboBox* combo_;
    QTreeWidget* tree_;

private slots:
    void on_combo_index_changed();
};

#endif // HISCORES_H

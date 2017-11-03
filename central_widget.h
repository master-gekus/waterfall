#ifndef CENTRAL_WIDGET_H
#define CENTRAL_WIDGET_H

#include <QWidget>

constexpr const int MIN_FIELD_SIZE = 4;
constexpr const int MAX_FIELD_SIZE = 10;

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = 0);

signals:

public slots:
};

#endif // CENTRAL_WIDGET_H

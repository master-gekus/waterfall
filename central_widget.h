﻿#ifndef CENTRAL_WIDGET_H
#define CENTRAL_WIDGET_H

#include <QWidget>

constexpr const int MIN_FIELD_SIZE = 4;
constexpr const int MAX_FIELD_SIZE = 10;

class QPushButton;

class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    enum State
    {
        Shadow,
        GamePlayWait,
        GamePlayAnimation,
        GameWin
    };
public:
    explicit CentralWidget(QWidget *parent = 0);

public:
    State state() const;
    int gameFieldSize() const;
    void setGameFieldSize(int new_size);
    QPushButton* newGameButton() const;
    void startNewGame();
    bool undoAvail() const;
    void undo();
    bool redoAvail() const;
    void redo();

signals:
    void animationFinished();
    void gameFinished(quint64 time_passed, quint32 clicks);

public slots:
};

#endif // CENTRAL_WIDGET_H

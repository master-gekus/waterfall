#include "central_widget.h"

#include <QLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QDateTime>
#include <QSet>
#include <QPoint>

Q_DECL_CONSTEXPR inline uint qHash(const QPoint& point, uint seed = 0)
{
    return qHash(point.x(), seed) ^ qHash(point.y() << 8, seed) ^ seed;
}

namespace
{
    class FieldButton : public QToolButton
    {
    public:
        enum State
        {
            Horizontal,
            HorizontalAnimated,
            Vertical,
            VerticalAnimated,
            Undefined
        };

        enum Directions {
            Up      = 0x01,
            Down    = 0x02,
            Left    = 0x04,
            Right   = 0x08,
            All     = (Up | Down | Left | Right),
        };
        Q_DECLARE_FLAGS(Direction, Directions)

    public:
        explicit FieldButton(CentralWidget* parent, int x, int y) :
            QToolButton(parent),
            x_(x), y_(y), state_(Undefined), stage_(0)
        {
            Q_ASSERT(!state_icons_[0].isNull());
            setIconSize(QSize(128, 128));
            setAutoRaise(true);
            setVisible(false);
            setIconByState();
        }

        bool updateStateByTimer()
        {
            switch(state_)
            {
            case HorizontalAnimated:
            case VerticalAnimated:
                stage_ = (stage_ + 1) % 6;
                break;
            default:
                return false;
            }
            if (0 == stage_) {
                state_ = (HorizontalAnimated == state_) ? Vertical : Horizontal;
            }
            setIconByState();
            return (0 == stage_);
        }

        void startAnimate()
        {
            switch(state_)
            {
            case Horizontal:
                state_ = HorizontalAnimated;
                break;
            case Vertical:
                state_ = VerticalAnimated;
                break;
            default:
                return;
            }
            stage_ = 1;
            setIconByState();
        }

        void inverse()
        {
            state_ = (Horizontal == state_) ? Vertical : Horizontal;
        }

        void setIconByState()
        {
            switch(state_)
            {
            case Horizontal:
                setIcon(state_icons_[0]);
                break;
            case HorizontalAnimated:
                setIcon(state_icons_[stage_]);
                break;
            case Vertical:
                setIcon(state_icons_[6]);
                break;
            case VerticalAnimated:
                setIcon(state_icons_[stage_ + 6]);
                break;
            case Undefined:
                setIcon(state_icons_[12]);
                break;
            default:
                return;
            }
            setEnabled(Undefined != state_);
        }

    public:
        int x_, y_;
        State state_;
        int stage_;
        Direction next_direction_;
        static QIcon state_icons_[13];
    };
    QIcon FieldButton::state_icons_[13];

    class CentralLayout : public QLayout
    {
    public:
        explicit CentralLayout(CentralWidget* parent) :
            QLayout(parent)
        {
            qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()));

            QPixmap state_icons;
            state_icons.load(QStringLiteral(":/res/bricks.png"));
            for (int i = 0; i < 13; i++) {
                FieldButton::state_icons_[i].addPixmap(state_icons.copy(i * 128, 0, 128, 128));
            }

            static const QString styleSheet("font-weight: bold; font-size: 12pt;");

            label_time_ = new QLabel(QStringLiteral("Время:"), parent);
            label_clicks_ = new QLabel(QStringLiteral("Кликов:"), parent);
            edit_time_ = new QLineEdit(QStringLiteral("0:00:00"), parent);
            edit_clicks_ = new QLineEdit(QStringLiteral("0"), parent);

            label_time_->setStyleSheet(styleSheet);
            label_clicks_->setStyleSheet(styleSheet);
            edit_time_->setStyleSheet(styleSheet);
            edit_clicks_->setStyleSheet(styleSheet);
            edit_time_->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
            edit_clicks_->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
            edit_time_->setReadOnly(true);
            edit_clicks_->setReadOnly(true);

            btn_new_game_ = new QPushButton(QStringLiteral("Новая игра"), parent);

            for (int i = 0; i < MAX_FIELD_SIZE * MAX_FIELD_SIZE; ++i) {
                btn_field_[i] = new FieldButton(parent, i % MAX_FIELD_SIZE, i / MAX_FIELD_SIZE);
                connect(btn_field_[i], &FieldButton::clicked, this, &CentralLayout::on_field_button_clicked);
            }

            animate_timer_.setSingleShot(false);
            animate_timer_.setInterval(50);
            animate_timer_.setTimerType(Qt::PreciseTimer);
            connect(&animate_timer_, &QTimer::timeout, this, &CentralLayout::on_animate_timer, Qt::QueuedConnection);

            gameplay_timer_.setSingleShot(false);
            gameplay_timer_.setInterval(1000);
            connect(&gameplay_timer_, &QTimer::timeout, this, &CentralLayout::update_game_stat, Qt::QueuedConnection);
        }

    public:
        int field_size_ = MIN_FIELD_SIZE;
        QLabel *label_time_, *label_clicks_;
        QLineEdit *edit_time_, *edit_clicks_;
        QPushButton *btn_new_game_;
        FieldButton* btn_field_[MAX_FIELD_SIZE * MAX_FIELD_SIZE];
        QTimer animate_timer_, gameplay_timer_;
        CentralWidget::State state_ = CentralWidget::Shadow;
        quint64 time_start_ = 0;
        quint32 clicks_ = 0;

    private:
        void update_game_stat()
        {
            edit_clicks_->setText(QString::number(clicks_));
            quint64 delta = (QDateTime::currentMSecsSinceEpoch() - time_start_ + 500) / 1000;
            edit_time_->setText(QStringLiteral("%1:%2:%3").arg(delta / 3600)
                                .arg((delta / 60) % 60, 2, 10, QChar('0'))
                                .arg(delta % 60, 2, 10, QChar('0')));
        }

    public:
        void startNewGame()
        {
            animate_timer_.stop();

            for (int i = 0; i < MAX_FIELD_SIZE * MAX_FIELD_SIZE; ++i) {
                btn_field_[i]->state_ = FieldButton::Horizontal;
            }

            // Заполним случайными значениями. Вообще говоря, для чётных размерностей полей
            // решение существует при любом раскладе. Но для нечётных размерностей мне не удалось
            // найти общего решения. Поэтому просто выполним случайное количество неповторяющихся
            // случайных ходов.
            while (true) {
                QSet<QPoint> steps;
                int size = field_size_ * field_size_;
                int count = (size / 4) + (qrand() % (size / 2));
                for (int i = 0; i < count; ++i) {
                    QPoint point(qrand() % field_size_, qrand() % field_size_);
                    if (steps.contains(point)) {
                        continue;
                    }
                    steps.insert(point);

                    for (int i = 0; i < field_size_; ++i) {
                        btn_field_[point.y() * MAX_FIELD_SIZE + i]->inverse();
                        btn_field_[i * MAX_FIELD_SIZE + point.x()]->inverse();
                    }
                    btn_field_[point.y() * MAX_FIELD_SIZE + point.x()]->inverse();
                }

                bool found = false;
                for (int x = 0; (!found) && (x < field_size_); ++ x) {
                    for (int y = 0; (!found) && (y < field_size_); ++y) {
                        if (FieldButton::Horizontal != btn_field_[y * MAX_FIELD_SIZE + x]->state_) {
                            found = true;
                        }
                    }
                }
                if (found) {
                    break;
                }
            }

            for (int i = 0; i < MAX_FIELD_SIZE * MAX_FIELD_SIZE; ++i) {
                btn_field_[i]->setIconByState();
            }
            state_ = CentralWidget::GamePlayWait;
            time_start_ = QDateTime::currentMSecsSinceEpoch();
            clicks_ = 0;
            gameplay_timer_.start();
            update_game_stat();
        }

    private:
        void addItem(QLayoutItem*) override
        {
            Q_ASSERT(false);
        }

        QLayoutItem* itemAt(int) const override
        {
            return nullptr;
        }

        QLayoutItem* takeAt(int) override
        {
            Q_ASSERT(false);
            return nullptr;
        }

        int count() const override
        {
            Q_ASSERT(false);
            return 0;
        }

        QSize sizeHint() const override
        {
            return QSize(100, 100);
        }

        void setGeometry(const QRect &rect) override
        {
            QSize hintEdit = edit_time_->sizeHint();
            QSize hintL1 = label_time_->sizeHint();
            QSize hintL2 = label_clicks_->sizeHint();
            int lw = qMax(hintL1.width(), hintL2.width());
            int ew = hintEdit.width() / 2;
            int eh = hintEdit.height();
            int d = eh / 4;
            int eo = rect.right() - d - ew;
            int lo = eo - d - lw;

            label_time_->setGeometry(lo, d, lw, eh);
            label_clicks_->setGeometry(lo, d * 2 + eh, lw, eh);
            edit_time_->setGeometry(eo, d, ew, eh);
            edit_clicks_->setGeometry(eo, d * 2 + eh, ew, eh);

            btn_new_game_->setGeometry(lo, d * 3 + eh * 2, lw + ew + d, eh);

            int fbs = qMin((lo - 2 * d) / field_size_,
                           (rect.height() - 2 * d) / field_size_);
            fbs = qMin(fbs, btn_field_[0]->sizeHint().width());
            int fbo = (lo - fbs * field_size_) / 2;
            for (int x = 0; x < MAX_FIELD_SIZE; x++)
            {
                for (int y = 0; y < MAX_FIELD_SIZE; y++)
                {
                    QToolButton *btn = btn_field_[y * MAX_FIELD_SIZE + x];
                    if ((x < field_size_) && (y < field_size_))
                    {
                        btn->setVisible(true);
                        btn->setGeometry(fbo + x * fbs, d + y * fbs, fbs, fbs);
                    } else {
                        btn->setVisible(false);
                    }
                }
            }
        }

    private:
        void on_field_button_clicked()
        {
            if (CentralWidget::GamePlayWait != state_) {
                return;
            }

            FieldButton *btn = dynamic_cast<FieldButton*>(sender());
            if (nullptr == btn) {
                return;
            }

            btn->next_direction_ = FieldButton::All;
            btn->startAnimate();
            state_ = CentralWidget::GamePlayAnimation;
            animate_timer_.start();

            clicks_++;
            update_game_stat();
        }

        void on_animate_timer()
        {
            if (CentralWidget::GamePlayAnimation != state_) {
                return;
            }

            QMap<FieldButton*, FieldButton::Direction> next_btns;
            bool animation_finished = false;
            for (int x = 0; x < field_size_; x++) {
                for (int y = 0; y < field_size_; y++) {
                    FieldButton *btn = btn_field_[y * MAX_FIELD_SIZE + x];
                    if (btn->updateStateByTimer())
                    {
                        animation_finished = true;
                        if ((0 != (btn->next_direction_ & FieldButton::Left)) && (x > 0)) {
                            next_btns.insert(btn_field_[y * MAX_FIELD_SIZE + x - 1], FieldButton::Left);
                        }
                        if ((0 != (btn->next_direction_ & FieldButton::Right)) && (x < (field_size_ - 1))) {
                            next_btns.insert(btn_field_[y * MAX_FIELD_SIZE + x + 1], FieldButton::Right);
                        }
                        if ((0 != (btn->next_direction_ & FieldButton::Up)) && (y > 0)) {
                            next_btns.insert(btn_field_[(y - 1) * MAX_FIELD_SIZE + x], FieldButton::Up);
                        }
                        if ((0 != (btn->next_direction_ & FieldButton::Down)) && (y < (field_size_ - 1))) {
                            next_btns.insert(btn_field_[(y + 1) * MAX_FIELD_SIZE + x], FieldButton::Down);
                        }
                    }
                }
            }
            for (auto it = next_btns.cbegin(); it != next_btns.cend(); ++it) {
                FieldButton *btn = it.key();
                btn->next_direction_ = it.value();
                btn->startAnimate();
            }
            if (animation_finished && next_btns.isEmpty()) {
                animate_timer_.stop();

                state_ = CentralWidget::GameWin;
                for (int x = 0; (CentralWidget::GameWin == state_) && (x < field_size_); x++) {
                    for (int y = 0; (CentralWidget::GameWin == state_) && (y < field_size_); y++) {
                        if (FieldButton::Horizontal != btn_field_[y * MAX_FIELD_SIZE + x]->state_) {
                            state_ = CentralWidget::GamePlayWait;
                        }
                    }
                }
                if (CentralWidget::GameWin == state_)
                {
                    CentralWidget *parent = dynamic_cast<CentralWidget*>(parentWidget());
                    Q_ASSERT(parent);

                    gameplay_timer_.stop();
                    update_game_stat();
                    emit
                        parent->gameFinished(QDateTime::currentMSecsSinceEpoch() - time_start_, clicks_);
                }
            }
        }
    };
}   // namespace;

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent)
{
    new CentralLayout(this);
}

int CentralWidget::gameFieldSize() const
{
    CentralLayout *l= dynamic_cast<CentralLayout*>(layout());
    Q_ASSERT(l);
    return l->field_size_;
}

void CentralWidget::setGameFieldSize(int new_size)
{
    CentralLayout *l= dynamic_cast<CentralLayout*>(layout());
    Q_ASSERT(l);

    if (l->field_size_ == new_size) {
        return;
    }

    if ((MIN_FIELD_SIZE > new_size) || (MAX_FIELD_SIZE < new_size)) {
        return;
    }

    l->field_size_ = new_size;

    if (Shadow != l->state_) {
        l->startNewGame();
    }

    l->update();
}

QPushButton* CentralWidget::newGameButton() const
{
    CentralLayout *l= dynamic_cast<CentralLayout*>(layout());
    Q_ASSERT(l);
    return l->btn_new_game_;
}

void CentralWidget::startNewGame()
{
    CentralLayout *l= dynamic_cast<CentralLayout*>(layout());
    Q_ASSERT(l);
    l->startNewGame();
}

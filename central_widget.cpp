#include "central_widget.h"

#include <QLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

namespace
{
    class FieldButton : public QToolButton
    {
    public:
        explicit FieldButton(CentralWidget* parent, int x, int y) :
            QToolButton(parent),
            x_(x), y_(y)
        {
            setIcon(QIcon(QStringLiteral(":/res/mainicon/128x128.png")));
            setIconSize(QSize(128, 128));
//            setAutoRaise(true);
            setVisible(false);
        }

    public:
        int x_, y_;
    };


    class CentralLayout : public QLayout
    {
    public:
        explicit CentralLayout(CentralWidget* parent) :
            QLayout(parent)
        {
            static const QString styleSheet("font-weight: bold; font-size: 12pt;");

            label_time_ = new QLabel(QStringLiteral("Время:"), parent);
            label_clicks_ = new QLabel(QStringLiteral("Кликов:"), parent);
            edit_time_ = new QLineEdit(QStringLiteral("0:00:00"), parent);
            edit_clicks_ = new QLineEdit(QStringLiteral("0"), parent);

            label_time_->setStyleSheet(styleSheet);
            label_clicks_->setStyleSheet(styleSheet);
            edit_time_->setStyleSheet(styleSheet);
            edit_clicks_->setStyleSheet(styleSheet);
            edit_time_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            edit_clicks_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            edit_time_->setReadOnly(true);
            edit_clicks_->setReadOnly(true);

            btn_new_game_ = new QPushButton(QStringLiteral("Новая игра"), parent);

            for (int i = 0; i < MAX_FIELD_SIZE * MAX_FIELD_SIZE; ++i)
                btn_field_[i] = new FieldButton(parent, i % MAX_FIELD_SIZE, i / MAX_FIELD_SIZE);
        }

    public:
        int field_size_ = MIN_FIELD_SIZE;
        QLabel *label_time_, *label_clicks_;
        QLineEdit *edit_time_, *edit_clicks_;
        QPushButton *btn_new_game_;
        FieldButton* btn_field_[MAX_FIELD_SIZE * MAX_FIELD_SIZE];

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
    l->update();
}

#include "central_widget.h"

#include <QLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

namespace
{
    const QString styleSheet("font-weight: bold; font-size: 12pt;");

    class CentralLayout : public QLayout
    {
    public:
        CentralLayout(QWidget* parent) :
            QLayout(parent)
        {
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
        }

    public:
        int field_size_ = 4;
        QLabel *label_time_, *label_clicks_;
        QLineEdit *edit_time_, *edit_clicks_;
        QPushButton *btn_new_game_;

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
        }
    };
}   // namespace;

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent)
{
    new CentralLayout(this);
}

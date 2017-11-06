#include "hiscores.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QComboBox>
#include <QTreeWidget>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVariant>

#include "central_widget.h"

Hiscores::Hiscores(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(QStringLiteral("Waterfall: Таблица рекордов"));

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);

    QFrame* frame = new QFrame(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(frame);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->addWidget(new QLabel(QStringLiteral("Показывать лучшие результаты для поля размером:"), frame));

    combo_ = new QComboBox(frame);
    horizontalLayout->addWidget(combo_);
    horizontalLayout->setStretch(1, 1);

    for (int i = MIN_FIELD_SIZE; i <= MAX_FIELD_SIZE; i++) {
        combo_->addItem(QStringLiteral("%1 x %1").arg(i), QVariant(i));
    }

    verticalLayout->addWidget(frame);

    tree_ = new QTreeWidget(this);
    tree_ ->setMinimumSize(QSize(382, 0));
    tree_ ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree_ ->setRootIsDecorated(false);
    tree_ ->setUniformRowHeights(true);
    tree_ ->setItemsExpandable(false);
    tree_ ->setAllColumnsShowFocus(true);
    tree_ ->setExpandsOnDoubleClick(false);
    tree_->setHeaderLabels(QStringList() << QStringLiteral("Имя")
                           << QStringLiteral("Время") << QStringLiteral("Кликов"));
    tree_->setColumnWidth(0, 180);
    tree_->setColumnWidth(1, 120);
    tree_->setColumnWidth(2, 70);

    verticalLayout->addWidget(tree_);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()),SLOT(close()));

    verticalLayout->addWidget(buttonBox);
}

Hiscores::~Hiscores()
{
}

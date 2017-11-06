#include "hiscores.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QComboBox>
#include <QTreeWidget>
#include <QLabel>
#include <QDialogButtonBox>
#include <QSettings>
#include <QVariant>
#include <QVariantList>
#include <QMap>
#include <QPair>

#include "central_widget.h"

namespace {
    class HiScoreTable
    {
    public:
        explicit HiScoreTable(int field_size) :
            field_size_(field_size)
        {
            QSettings settings;
            settings.beginGroup(key_section_);
            int count = settings.beginReadArray(key_subsection_.arg(field_size));
            for (int i = 0; i < count; ++i) {
                settings.setArrayIndex(i);
                QString name = settings.value(key_name_).toString();
                bool ok;
                quint64 time = settings.value(key_time_).toULongLong(&ok);
                if (!ok) {
                    continue;
                }
                quint32 clicks = settings.value(key_clicks_).toUInt(&ok);
                if (!ok) {
                    continue;
                }
                scores_.insert(QPair<quint64, quint32>(time, clicks), name);
            }

            while (scores_.size() > 10) {
                scores_.remove(scores_.lastKey());
            }
        }

        ~HiScoreTable()
        {
            QSettings settings;
            settings.beginGroup(key_section_);
            settings.beginWriteArray(key_subsection_.arg(field_size_), scores_.size());
            int count = 0;
            for (auto it = scores_.cbegin(); it != scores_.cend(); ++it, ++count)
            {
                settings.setArrayIndex(count);
                settings.setValue(key_name_, it.value());
                settings.setValue(key_time_, it.key().first);
                settings.setValue(key_clicks_, it.key().second);
            }
        }

        void fillTable(QTreeWidget *tree)
        {
            tree->clear();
            for (auto it = scores_.cbegin(); it != scores_.cend(); ++it)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText(0, it.value());
                item->setText(1, Hiscores::timeToString(it.key().first));
                item->setTextAlignment(1, Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
                item->setText(2, QString::number(it.key().second));
                item->setTextAlignment(2, Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
                tree->addTopLevelItem(item);
            }
        }

        bool isInHiScore(quint64 time, quint32 clicks)
        {
            return (10 > scores_.size())
                   || (!(scores_.lastKey() < QPair<quint64, quint32>(time, clicks)));
        }

        void addHiScore(const QString& name, quint64 time, quint32 clicks)
        {
            scores_.insert(QPair<quint64, quint32>(time, clicks), name);
            while (scores_.size() > 10) {
                scores_.remove(scores_.lastKey());
            }
        }

    private:
        int field_size_;
        QMap<QPair<quint64, quint32>, QString> scores_;
        static const QString& key_section_;
        static const QString& key_subsection_;
        static const QString& key_name_;
        static const QString& key_time_;
        static const QString& key_clicks_;
    };

    const QString& HiScoreTable::key_section_ = QStringLiteral("HiScorest");
    const QString& HiScoreTable::key_subsection_ = QStringLiteral("%1x%1");
    const QString& HiScoreTable::key_name_ = QStringLiteral("Name");
    const QString& HiScoreTable::key_time_ = QStringLiteral("Time");
    const QString& HiScoreTable::key_clicks_ = QStringLiteral("Clicks");
}

Hiscores::Hiscores(QWidget *parent, int field_size) :
    QDialog(parent)
{
    setWindowTitle(QStringLiteral("Waterfall: Таблица рекордов"));
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

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
        combo_->addItem(QStringLiteral("%1 x %1").arg(i));
    }
    combo_->setCurrentIndex(field_size - MIN_FIELD_SIZE);

    verticalLayout->addWidget(frame);

    tree_ = new QTreeWidget(this);
    tree_ ->setMinimumSize(QSize(382, 0));
    tree_ ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree_ ->setRootIsDecorated(false);
    tree_ ->setUniformRowHeights(true);
    tree_ ->setItemsExpandable(false);
    tree_ ->setAllColumnsShowFocus(true);
    tree_ ->setExpandsOnDoubleClick(false);
    tree_->setAlternatingRowColors(true);
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

    connect(combo_, SIGNAL(currentIndexChanged(int)), SLOT(on_combo_index_changed()));
    on_combo_index_changed();
}

Hiscores::~Hiscores()
{
}

void Hiscores::on_combo_index_changed()
{
    HiScoreTable(combo_->currentIndex() + MIN_FIELD_SIZE).fillTable(tree_);
}

QString Hiscores::timeToString(quint64 time)
{
    time += 500;
    time /= 1000;
    return QStringLiteral("%1:%2:%3").arg(time / 3600)
                        .arg((time / 60) % 60, 2, 10, QChar('0'))
                        .arg(time % 60, 2, 10, QChar('0'));
}

bool Hiscores::isInHiScore(int field_size, quint64 time, quint32 clicks)
{
    return HiScoreTable(field_size).isInHiScore(time, clicks);
}

void Hiscores::addHiScore(int field_size, const QString& name, quint64 time, quint32 clicks)
{
    HiScoreTable(field_size).addHiScore(name, time, clicks);
}

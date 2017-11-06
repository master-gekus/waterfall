#include "mainwindow.h"

#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDir>
#include <QIcon>
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QSettings>

#include "central_widget.h"
#include "hiscores.h"

namespace
{
    class AboutBox : public QDialog
    {
    public:
        explicit AboutBox(QWidget *parent = 0) :
            QDialog(parent)
        {
            setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
            QVBoxLayout* layout_main = new QVBoxLayout(this);
            layout_main->setSizeConstraint(QLayout::SetFixedSize);

            QFrame* frame_main = new QFrame(this);
            QHBoxLayout* layout_info = new QHBoxLayout(frame_main);
            layout_info->setContentsMargins(0, 0, 0, 0);

            QFrame* frame_icon = new QFrame(frame_main);
            QVBoxLayout* layout_icon = new QVBoxLayout(frame_icon);
            layout_icon->setContentsMargins(0, 0, 32, 0);

            QLabel* label_icon = new QLabel(frame_icon);
            label_icon->setPixmap(QPixmap(QString::fromUtf8(":/res/mainicon/128x128.png")));
            layout_icon->addWidget(label_icon);
            layout_icon->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

            layout_info->addWidget(frame_icon);

            QFrame*  frame_text = new QFrame(frame_main);
            QVBoxLayout* layout_text = new QVBoxLayout(frame_text);
            layout_text->setContentsMargins(0, 0, 0, 0);
            QLabel* label_appname = new QLabel(QApplication::applicationName(), frame_text);
            label_appname->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 12pt;"));
            label_appname->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

            layout_text->addWidget(label_appname);

            QLabel* label_version = new QLabel(QApplication::applicationVersion(), frame_text);
            label_version->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
            label_version->setWordWrap(true);

            layout_text->addWidget(label_version);

            QLabel* label_4 = new QLabel(frame_text);
            label_4->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
            label_4->setWordWrap(false);

            layout_text->addWidget(label_4);

            layout_text->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

            layout_info->addWidget(frame_text);

            layout_main->addWidget(frame_main);

            QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
            buttonBox->setOrientation(Qt::Horizontal);
            buttonBox->setStandardButtons(QDialogButtonBox::Ok);
            layout_main->addWidget(buttonBox);

            QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        }
    };
}   // namespace

QIcon MainWindow::main_icon_;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    QSettings settings;

    QDir folder(QStringLiteral(":/res/mainicon"));
    for(const QString& file_name : folder.entryList())
        main_icon_.addFile(folder.filePath(file_name));
    QApplication::setWindowIcon(main_icon_);

    setWindowTitle(QStringLiteral("Waterfall"));

    setObjectName(QStringLiteral("MainWindow"));
    setMinimumSize(QSize(640, 480));

    game_field_ = new CentralWidget(this);
    setCentralWidget(game_field_);

    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuGame = new QMenu(QStringLiteral("Игра"), menuBar);

    field_size_actions_ = new QAction*[MAX_FIELD_SIZE - MIN_FIELD_SIZE + 1];
    field_size_group_ = new QActionGroup(this);
    game_field_->setGameFieldSize(settings.value(QStringLiteral("Field Size")).toInt());
    int cur_field_size = game_field_->gameFieldSize();
    QMenu *menuFieldSize = new QMenu(QStringLiteral("Размер поля"), this);
    for (int i = MIN_FIELD_SIZE; i <= MAX_FIELD_SIZE; ++i)
    {
        QAction *action = new QAction(QStringLiteral("%1 x %1").arg(i), this);
        menuFieldSize->addAction(action);
        action->setData(QVariant(i));
        action->setCheckable(true);
        field_size_group_->addAction(action);
        if (i == cur_field_size)
            action->setChecked(true);
        connect(action, SIGNAL(triggered(bool)), SLOT(on_action_field_size()));
        field_size_actions_[i - MIN_FIELD_SIZE] = action;
    }
    menuGame->addAction(menuFieldSize->menuAction());
    menuGame->addSeparator();

    QAction *actionGameNew = new QAction(QStringLiteral("Новая"), this);
    actionGameNew->setShortcut(Qt::Key_F2);
    menuGame->addAction(actionGameNew);

    QAction *actionHiscores = new QAction(QStringLiteral("Таблица рекордов..."), this);
    menuGame->addAction(actionHiscores);
    menuGame->addSeparator();

    QAction *actionQuit = new QAction(QStringLiteral("Выход"), this);
    actionQuit->setMenuRole(QAction::QuitRole);
    menuGame->addAction(actionQuit);

    QMenu* menuHelp = new QMenu(QStringLiteral("Помощь"), menuBar);
    QAction *actionAbout = new QAction(QStringLiteral("О программе..."), this);
    actionAbout->setMenuRole(QAction::AboutRole);
    menuHelp->addAction(actionAbout);

    menuBar->addAction(menuGame->menuAction());
    menuBar->addAction(menuHelp->menuAction());
    setMenuBar(menuBar);

    restoreGeometry(settings.value(QStringLiteral("Main Window Geometry")).toByteArray());

    connect(actionQuit, SIGNAL(triggered(bool)), SLOT(close()));
    connect(actionAbout, SIGNAL(triggered(bool)), SLOT(on_action_about()));
    connect(actionGameNew, SIGNAL(triggered(bool)), SLOT(on_action_game_new()));
    connect(actionHiscores, SIGNAL(triggered(bool)), SLOT(on_action_hiscores()));
    connect(game_field_->newGameButton(), SIGNAL(clicked(bool)), SLOT(on_action_game_new()));
    connect(game_field_, SIGNAL(gameFinished(quint64,quint32)), SLOT(on_game_finished(quint64,quint32)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(QStringLiteral("Field Size"), game_field_->gameFieldSize());
    settings.setValue(QStringLiteral("Main Window Geometry"), saveGeometry());

    QMainWindow::closeEvent(event);
}

void MainWindow::on_action_about()
{
    AboutBox(this).exec();
}

void MainWindow::on_action_field_size()
{
    QAction *action = dynamic_cast<QAction*>(sender());
    Q_ASSERT(action);
    game_field_->setGameFieldSize(action->data().toInt());
}

void MainWindow::on_action_game_new()
{
    game_field_->startNewGame();
}

namespace
{
    const QString& switch_mes(uint value, const QString& one, const QString& two, const QString& many)
    {
        if((10 < value) && (20 > value)) {
            return many;
        }
        switch (value % 10) {
        case 1:
            return one;
        case 2:
        case 3:
        case 4:
            return two;
        default:
            return many;
        }
    }
}

void MainWindow::on_game_finished(quint64 msecs_passed, quint32 clicks)
{
    quint64 secs = (msecs_passed + 500) / 1000;
    QString str_time;
    if (3600 <= secs) {
        uint hours = secs / 3600;
        str_time = QStringLiteral("%1 %2 ").arg(hours).arg(switch_mes(hours,
            QStringLiteral("час"), QStringLiteral("часа"), QStringLiteral("часов")));
    }

    if (60 <= secs) {
        uint minutes = (secs / 60) % 60;
        str_time += QStringLiteral("%1 %2 ").arg(minutes).arg(switch_mes(minutes,
            QStringLiteral("минуту"), QStringLiteral("минуты"), QStringLiteral("минут")));
    }

    uint seconds = secs % 60;
    str_time += QStringLiteral("%1 %2 и %3 %4").arg(seconds).arg(switch_mes(seconds,
        QStringLiteral("секунду"), QStringLiteral("секунды"), QStringLiteral("секунд")))
        .arg(clicks).arg(switch_mes(clicks,
        QStringLiteral("клик"), QStringLiteral("клика"), QStringLiteral("кликов")));
    str_time = QStringLiteral("Вы успешно решили головоломку за %1!").arg(str_time);

    int field_size = game_field_->gameFieldSize();

    if (Hiscores::isInHiScore(field_size, msecs_passed, clicks)) {
        static const QString& key_user_name = QStringLiteral("User name");
        QSettings settings;
        QString name = settings.value(key_user_name).toString();
        if (name.isEmpty()) {
            QByteArray user_name = qgetenv("USER");
            if (user_name.isEmpty()) {
                user_name = qgetenv("USERNAME");
            }
            if (user_name.isEmpty()) {
                name = QStringLiteral("Мудрый игрок");
            }
            else {
                name = QString::fromUtf8(user_name);
            }
        }
        bool ok;
        name = QInputDialog::getText(this, QStringLiteral("Поздравляем!"), str_time +
            QStringLiteral("\r\nС этим результатом Вы достойны быть увековеченны в таблице рекордов!"
                           "\r\nПожалуйста, введите Ваше имя:"), QLineEdit::Normal, name, &ok);
        if (!ok) {
            return;
        }
        settings.setValue(key_user_name, name);
        Hiscores::addHiScore(field_size, name, msecs_passed, clicks);
        QTimer::singleShot(0, this, &MainWindow::on_action_hiscores);
    }
    else {
        QMessageBox::information(this, QStringLiteral("Поздравляем!"), str_time +
            QStringLiteral("\r\nК сожалению, этого этого результата не достаточно, "
                           "чтобы оставить о себе память в таблице рекордов."));
    }
}

void MainWindow::on_action_hiscores()
{
    Hiscores(this, game_field_->gameFieldSize()).exec();
}


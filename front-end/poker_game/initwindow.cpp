#include "initwindow.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include "login_view.h"
#include <QTranslator>
#include <QApplication>
#include <QFontDatabase>
#include <QProgressBar>

InitWindow::InitWindow(QWidget *parent)
    : QWidget(parent)
{
    auto init_label = new QLabel(this);
    init_label->setText("Initialing");
    init_label->setAlignment(Qt::AlignCenter);

    auto hblayout = new QHBoxLayout();
    hblayout->addStretch(1);
    hblayout->addWidget(init_label, 1);
    hblayout->addStretch(1);



    auto vblayout = new QVBoxLayout(this);
    vblayout->addStretch(1);
    vblayout->addLayout(hblayout, 1);
    vblayout->addStretch(1);

    m_pgbar = new QProgressBar(this);
    vblayout->addStretch(1);

    vblayout->addWidget(m_pgbar, 1);
    auto cancel_btn = new QPushButton(this);
    cancel_btn->setText("Cancel");
    vblayout->addWidget(cancel_btn, 1);
    vblayout->addStretch(1);

    connect(cancel_btn, SIGNAL(clicked()), this, SLOT(stop_init()));
}

InitWindow::~InitWindow()
{
}

void InitWindow::win_resouce_request()
{
    QFontDatabase qd;
    if (false == qd.hasFamily("Microsoft YaHei Light"))
    {
        auto font_url = QUrl(QUrl::fromUserInput(QString("http://") + RESOURCE_HOST_NAME + INIT_FONT_FILE_URL));
        qDebug("%s", font_url.toDisplayString().toUtf8().data());
        QNetworkRequest req(font_url);

        m_poReply = nam.get(req);
        connect(m_poReply, SIGNAL(finished()), this, SLOT(win_resouce_recv()));
        connect(m_poReply, SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateDataReadProgress(qint64,qint64)));
    }
    else
    {
        QFont font("Microsoft YaHei Light");
        font.setPointSize(16);
        qApp->setFont(font);

        auto lv = new Login_view(this);
        lv->show();
        this->hide();
    }

}

void InitWindow::stop_init()
{
    this->close();
}

void InitWindow::win_resouce_recv()
{
    auto ft_content = m_poReply->readAll();
    QFontDatabase::addApplicationFontFromData(ft_content);

    QFont font("Microsoft YaHei Light");
    font.setPointSize(12);
    qApp->setFont(font);

    auto lv = new Login_view(this);
    lv->show();
    this->hide();
}

void InitWindow::updateDataReadProgress(qint64 cur, qint64 tot)
{
    m_pgbar->setRange(0, tot);
    m_pgbar->setValue(cur);
}


#include "login_view.h"
#include "QLabel"
#include <QTranslator>
#include <QVBoxLayout>
#include <QPushButton>
#include "register_view.h"
#include "game/game_view.h"
#include "rest_api_json.h"


Login_view::Login_view(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle(tr("登录"));

    auto cenwid = new QWidget(this);
    auto cenlayout = new QVBoxLayout(cenwid);
    setCentralWidget(cenwid);

//    auto user_name_layout = new QHBoxLayout();
//    cenlayout->addLayout(user_name_layout, 2);
//    user_name_layout->addWidget(new QLabel(tr("用户名"),this), 1);
//    m_username_input = new QLineEdit(this);
//    user_name_layout->addWidget(m_username_input, 2);

//    auto password_layout = new QHBoxLayout();
//    cenlayout->addLayout(password_layout, 2);
//    password_layout->addWidget(new QLabel(tr("密码"), this), 1);
//    m_password_input = new QLineEdit(this);
//    m_password_input->setEchoMode(QLineEdit::Password);
//    password_layout->addWidget(m_password_input, 2);

//    auto submit_layout = new QHBoxLayout();
//    cenlayout->addLayout(submit_layout, 1);

//    m_login_btn = new QPushButton(tr("登录"), this);
//    connect(m_login_btn, SIGNAL(clicked()), this, SLOT(login_btn_click()));

//    m_cancel_btn = new QPushButton(tr("退出"), this);
//    connect(m_cancel_btn, SIGNAL(clicked()), this, SLOT(cancel_btn_click()));

//    submit_layout->addWidget(m_login_btn);
//    submit_layout->addWidget(m_cancel_btn);

//    m_register_btn = new QPushButton(tr("注册"),this);
//    connect(m_register_btn, SIGNAL(clicked()), this, SLOT(regist_btn_click()));
//    cenlayout->addWidget(m_register_btn, 1);

    m_random_login_btn = new QPushButton("游客登录", this);
    connect(m_random_login_btn, SIGNAL(clicked()), this, SLOT(random_login_btn()));
    cenlayout->addWidget(m_random_login_btn);
}

void Login_view::login_btn_click()
{

}

void Login_view::cancel_btn_click()
{
    this->close();
}

void Login_view::regist_btn_click()
{
    auto rv_win = new register_view(this);
    rv_win->show();
}

void Login_view::random_login_btn()
{
    auto pnmh = new network_rest_util(this,this, "/user_manage/login_random");
    pnmh->req_get();
}

void Login_view::recv_data(QByteArray _data, QString _orig_uri, IprocessRestReply::orig_method _method, QByteArray _orig_req_data)
{
    QJsonDocument json_doc = QJsonDocument::fromJson(_data);
    auto reg_resp = json_doc.object();
    if (reg_resp.contains(REST_JSON_TYPE_KEY) && reg_resp[REST_JSON_TYPE_KEY] == REST_JSON_TYPE_RESP)
    {
        if (reg_resp.contains(REST_JSON_STATUS_KEY) && reg_resp[REST_JSON_STATUS_KEY] == REST_JSON_STATUS_SUCCESS)
        {
            QString ssid = reg_resp[REST_JSON_RANDOMLOGIN_SSID].toString();
            game_control::GetController()->ConnectToGame(ssid);
        }
    }
}

void Login_view::meet_error(QNetworkReply::NetworkError _code, QString _orig_uri, IprocessRestReply::orig_method _method, QByteArray _orig_req_data)
{

}


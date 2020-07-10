#include "register_view.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QJsonDocument>
#include "rest_api_json.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

register_view::register_view(QWidget *parent) : QMainWindow(parent)
{
    this->setWindowTitle(tr("注册"));

    auto cenwid = new QWidget(this);
    auto cenlayout = new QVBoxLayout(cenwid);
    setCentralWidget(cenwid);

    auto user_name_layout = new QHBoxLayout();
    cenlayout->addLayout(user_name_layout, 2);
    user_name_layout->addWidget(new QLabel(tr("用户名"),this), 1);
    m_username_input = new QLineEdit(this);
    user_name_layout->addWidget(m_username_input, 2);

    auto password_layout = new QHBoxLayout();
    cenlayout->addLayout(password_layout, 2);
    password_layout->addWidget(new QLabel(tr("密码"), this), 1);
    m_password_input = new QLineEdit(this);
    m_password_input->setEchoMode(QLineEdit::Password);
    password_layout->addWidget(m_password_input, 2);

    auto submit_layout = new QHBoxLayout();
    cenlayout->addLayout(submit_layout, 1);
    m_register_btn = new QPushButton(tr("提交"), this);
    m_cancel_btn = new QPushButton(tr("取消"), this);
    submit_layout->addWidget(m_register_btn);
    submit_layout->addWidget(m_cancel_btn);
    connect(m_register_btn, SIGNAL(clicked()), this, SLOT(register_btn_click()));
    connect(m_cancel_btn, SIGNAL(clicked()), this, SLOT(cancel_btn_click()));
}

void register_view::register_btn_click()
{
    auto username = m_username_input->text();
    auto password = m_password_input->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::information(this, tr("注册失败"), tr("请输入合适的用户名或密码"));
        return;
    }

    QJsonObject reg_req;

    reg_req[REST_JSON_TYPE_KEY] = REST_JSON_TYPE_REQ;
    reg_req[REST_JSON_ACTION_KEY] = REST_JSON_ACTION_REG;
    reg_req[REST_JSON_REG_USERNAME_KEY] = username;
    reg_req[REST_JSON_REG_USERNAME_PASSWORD] = password;

    qDebug("%s input username:%s", __func__, username.toUtf8().data());
    qDebug("%s input password:%s", __func__, password.toUtf8().data());

    QJsonDocument json_doc = QJsonDocument(reg_req);
    auto net_util = new network_rest_util(this, this, "/user_manage/register", json_doc.toJson());
    net_util->req_post();
}

void register_view::cancel_btn_click()
{
    this->close();
}

void register_view::recv_data(QByteArray _data, QString _orig_uri, IprocessRestReply::orig_method _method, QByteArray _orig_req_data)
{
    (void)(_orig_uri);
    (void)(_method);
    (void)(_orig_req_data);

    QJsonDocument json_doc = QJsonDocument::fromJson(_data);
    auto reg_resp = json_doc.object();
    if (reg_resp.contains(REST_JSON_TYPE_KEY) && reg_resp[REST_JSON_TYPE_KEY] == REST_JSON_TYPE_RESP)
    {
        if (reg_resp.contains(REST_JSON_STATUS_KEY) && reg_resp[REST_JSON_STATUS_KEY] == REST_JSON_STATUS_SUCCESS)
        {
            QMessageBox::information(this, tr("成功"), tr("注册成功"));
        }
    }
}

void register_view::meet_error(QNetworkReply::NetworkError _code, QString _orig_uri, IprocessRestReply::orig_method _method, QByteArray _orig_req_data)
{
    (void)(_code);
    (void)(_orig_uri);
    (void)(_method);
    (void)(_orig_req_data);
    QMessageBox::information(this, tr("失败"), tr("注册失败"));
    return;
}

#ifndef LOGIN_VIEW_H
#define LOGIN_VIEW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QButtonGroup>
#include "network_rest_util.h"

class Login_view : public QMainWindow, public IprocessRestReply
{
    Q_OBJECT
public:
    explicit Login_view(QWidget *parent = nullptr);

private:
    QLineEdit *m_username_input = NULL;
    QLineEdit *m_password_input = NULL;
    QPushButton *m_login_btn = NULL;
    QPushButton *m_cancel_btn = NULL;
    QPushButton *m_register_btn = NULL;
    QPushButton *m_random_login_btn = NULL;

private slots:
    void login_btn_click();
    void cancel_btn_click();
    void regist_btn_click();
    void random_login_btn();

signals:


    // IprocessRestReply interface
public:
    void recv_data(QByteArray _data, QString _orig_uri, orig_method _method, QByteArray _orig_req_data);
    void meet_error(QNetworkReply::NetworkError _code, QString _orig_uri, orig_method _method, QByteArray _orig_req_data);
};

#endif // LOGIN_VIEW_H

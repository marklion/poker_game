#ifndef REGISTER_VIEW_H
#define REGISTER_VIEW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include "network_rest_util.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class register_view : public QMainWindow, public IprocessRestReply
{
    Q_OBJECT
public:
    explicit register_view(QWidget *parent = nullptr);

private:
    QLineEdit *m_username_input = NULL;
    QLineEdit *m_password_input = NULL;
    QPushButton *m_register_btn = NULL;
    QPushButton *m_cancel_btn = NULL;

private slots:
    void register_btn_click();
    void cancel_btn_click();

signals:


    // IprocessRestReply interface
public:
    void recv_data(QByteArray _data, QString _orig_uri, orig_method _method, QByteArray _orig_req_data);
    void meet_error(QNetworkReply::NetworkError _code, QString _orig_uri, orig_method _method, QByteArray _orig_req_data);
};

#endif // REGISTER_VIEW_H

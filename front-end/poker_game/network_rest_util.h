#ifndef NETWORK_REST_UTIL_H
#define NETWORK_REST_UTIL_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#define REMOTE_REST_HOST "59.110.64.232"

class IprocessRestReply
{
public:
    enum orig_method {
        get_req,post_req
    };
    virtual void recv_data(QByteArray _data, QString _orig_uri, orig_method _method, QByteArray _orig_req_data = QByteArray()) = 0;
    virtual void meet_error(QNetworkReply::NetworkError _code, QString _orig_uri, orig_method _method, QByteArray _orig_req_data = QByteArray()) = 0;
};



class network_rest_util : public QObject
{
    Q_OBJECT
public:
    explicit network_rest_util(QObject *parent = nullptr,
                               IprocessRestReply *_processor = nullptr,
                               QString _uri = QString(),
                               QByteArray _data = QByteArray());
    bool req_get();
    bool req_post();
private:
    QNetworkAccessManager m_nwam;
    QNetworkReply *m_reply = NULL;
    IprocessRestReply *m_processor = NULL;
    QString m_uri;
    QByteArray m_data;
    IprocessRestReply::orig_method m_req_method;

private slots:
    void network_recv();
    void network_error(QNetworkReply::NetworkError _code);

signals:

};

#endif // NETWORK_REST_UTIL_H

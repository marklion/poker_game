#include "network_rest_util.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>




network_rest_util::network_rest_util(QObject *parent, IprocessRestReply *_processor, QString _uri, QByteArray _data)
    : QObject(parent), m_processor(_processor), m_uri(_uri), m_data(_data)
{
    qDebug("%s construct data:%s", __func__, _data.data());
}

bool network_rest_util::req_get()
{
    bool ret = false;

    m_req_method = IprocessRestReply::get_req;
    QNetworkRequest get_req = QNetworkRequest(QUrl(QUrl::fromUserInput(QString("http://") + REMOTE_REST_HOST + m_uri)));
    m_reply = m_nwam.get(get_req);
    qDebug("%s send get : %s", __func__, get_req.url().toDisplayString().toUtf8().data());

    if (nullptr != m_reply)
    {
        connect(m_reply, SIGNAL(finished()), this, SLOT(network_recv()));
        connect(m_reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(network_error(QNetworkReply::NetworkError)));
        ret = true;
    }
    else
    {
        qDebug(__func__);
    }

    return ret;
}

bool network_rest_util::req_post()
{
    bool ret = false;

    auto json_doc = QJsonDocument::fromJson(m_data);
    auto orig_json = json_doc.object();
    QJsonObject pre_json;
    pre_json["text"] = orig_json;
    json_doc.setObject(pre_json);
    m_data = json_doc.toJson();

    m_req_method = IprocessRestReply::post_req;
    auto request = QNetworkRequest(QUrl(QUrl::fromUserInput(QString("http://") + REMOTE_REST_HOST + m_uri)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug("%s send post:%s data: %s", __func__, request.url().toDisplayString().toUtf8().data(), m_data.data());

    m_reply = m_nwam.post(request, m_data);
    qDebug("after send post");
    if (nullptr != m_reply)
    {
        connect(m_reply, SIGNAL(finished()), this, SLOT(network_recv()));
        connect(m_reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(network_error(QNetworkReply::NetworkError)));
        ret = true;
    }
    else
    {
        qDebug(__func__);
    }

    return ret;
}



void network_rest_util::network_recv()
{
    auto recv_data = m_reply->readAll();
    qDebug("%s recv data:%s", __func__, recv_data.data());

    auto orig_json_doc = QJsonDocument::fromJson(recv_data);
    auto orig_json = orig_json_doc.object();
    QJsonDocument need_json_doc(orig_json["result"].toObject());

    this->m_processor->recv_data(need_json_doc.toJson(),m_uri, m_req_method, m_data);
}

void network_rest_util::network_error(QNetworkReply::NetworkError _code)
{
    qDebug("%s:%d\n", __func__, _code);
    this->m_processor->meet_error(_code, m_uri, m_req_method, m_data);
}


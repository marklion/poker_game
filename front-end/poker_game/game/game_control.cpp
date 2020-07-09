#include "game_view.h"
#include "game_control.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "msg-encode-utils/msg_encode_utils.h"
#include <cstring>
#include <QTimer>
#include <QMessageBox>

game_control *game_control::m_inst = NULL;

game_control *game_control::GetController()
{
    if (NULL == m_inst)
    {
        m_inst = new game_control();
    }

    return m_inst;
}

void game_control::AddView(refresh_view *_view)
{
    m_view_list.push_back(_view);
}

void game_control::DelView(refresh_view *_view)
{
    m_view_list.removeOne(_view);
}

bool game_control::ConnectToGame(QString _ssid)
{
    m_ssid = _ssid;
    QNetworkRequest req(QUrl::fromUserInput(QString("ws://") + REMOTE_REST_HOST + "/game"));
    req.setRawHeader("Sec-WebSocket-Protocol", "binary");
    m_ws.open(req);
    connect(&m_ws, SIGNAL(connected()), this, SLOT(proc_game_connect()));
    connect(&m_ws, SIGNAL(disconnected()), this, SLOT(proc_game_disconnect()));
    connect(&m_ws, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(proc_error(QAbstractSocket::SocketError)));
    connect(&m_ws, SIGNAL(binaryMessageReceived(const QByteArray)), this, SLOT(proc_recv_msg(const QByteArray)));

    return true;
}

void game_control::ReqToFollowTable(int _no)
{
    game_msg_enter_table msg;
    msg.iTableId = _no;

    int sendlen = msg_GetBuffLen(game_msg_type_enter_table);
    char *sendbuff = (char *)calloc(1UL, sendlen);
    MSG_ENCODE(msg, m_ssid.toUtf8().data(), game_msg_type_enter_table, sendbuff);

    m_ws.sendBinaryMessage(QByteArray::fromRawData(sendbuff, sendlen));
    free(sendbuff);
}

void game_control::ReqToCreateTable()
{
    game_msg_create_table msg;

    int sendlen = msg_GetBuffLen(game_msg_type_create_table);
    char *sendbuff = (char *)calloc(1UL, sendlen);
    MSG_ENCODE(msg, m_ssid.toUtf8().data(), game_msg_type_create_table, sendbuff);

    m_ws.sendBinaryMessage(QByteArray::fromRawData(sendbuff, sendlen));
    free(sendbuff);
}

void game_control::ReqToGetReady(bool _ready)
{
    game_msg_ready msg;

    msg.bReady = _ready;
    int sendlen = msg_GetBuffLen(game_msg_type_ready);
    char *sendbuff = (char *)calloc(1UL, sendlen);
    MSG_ENCODE(msg, m_ssid.toUtf8().data(), game_msg_type_ready, sendbuff);

    m_ws.sendBinaryMessage(QByteArray::fromRawData(sendbuff, sendlen));
    free(sendbuff);
}

void game_control::ReqToBatCash(game_msg_action_em _act, int _iCash)
{
    game_msg_action msg;
    msg.iCash = _iCash;
    msg.enAction = _act;
    int sendlen = msg_GetBuffLen(game_msg_type_action);
    char *sendbuff = (char *)calloc(1UL, sendlen);
    MSG_ENCODE(msg, m_ssid.toUtf8().data(), game_msg_type_action, sendbuff);

    m_ws.sendBinaryMessage(QByteArray::fromRawData(sendbuff, sendlen));
    free(sendbuff);
}

game_control::game_control(QObject *parent) : QObject(parent)
{

}

void game_control::proc_game_connect()
{
    auto gamev = new game_view();
    gamev->show();
    m_heart_timer.start(10000);
    connect(&m_heart_timer, SIGNAL(timeout()), this, SLOT(proc_heart_timeout()));
    m_refresh_timer.start(1000);
    connect(&m_refresh_timer, SIGNAL(timeout()), this, SLOT(proc_refresh_timeout()));

}

void game_control::proc_game_disconnect()
{
    QMessageBox::information(nullptr, "游戏结束", "筹码不够了，请点击退出");
    qDebug("---------close--------");
}

void game_control::proc_error(QAbstractSocket::SocketError _err)
{
    qDebug("+++++%d++++++", _err);
}

void game_control::proc_recv_msg(const QByteArray &_msg)
{
    const char *inputBuff = _msg.data();
    int inputLen = _msg.length();

    recv_and_proccess_msg(inputBuff, inputLen, this);

    qDebug("r::::%s", _msg.toHex().data());
    for (auto vitr:m_view_list)
    {
        vitr->refresh();
    }
}

void game_control::proc_heart_timeout()
{
    game_msg_heart msg;
    strcpy(msg.content, "ping");

    int sendlen = msg_GetBuffLen(game_msg_type_heart);
    char *buff = (char *)calloc(1UL, sendlen);
    MSG_ENCODE(msg, m_ssid.toUtf8().data(), game_msg_type_heart, buff);
    m_ws.sendBinaryMessage(QByteArray::fromRawData(buff, sendlen));
    free(buff);

}

void game_control::proc_refresh_timeout()
{

    for (auto vitr:m_view_list)
    {
        vitr->refresh();
    }
}

void game_control::proc_msg(game_msg_table_info &_msg, char *ssid)
{
    m_game_module.SyncTableInfo(_msg);
}

void game_control::proc_msg(game_msg_hand_card_info &_msg, char *ssid)
{
    m_game_module.UpdateMyself(_msg);
}

void game_control::proc_msg(game_msg_last_result &_msg, char *ssid)
{
    m_game_module.UpdateResult(_msg);
}



#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <QObject>
#include "game_module.h"
#include "network_rest_util.h"
#include <QWebSocket>
#include "msg-encode-utils/msg_encode_utils.h"
#include <QTimer>

class refresh_view;

class game_control : public QObject, public Imsg_processor
{
    Q_OBJECT
public:
    static game_control *GetController();
    void AddView(refresh_view *_view);
    void DelView(refresh_view *_view);
    game_module m_game_module;
    bool ConnectToGame(QString _ssid);
    void ReqToFollowTable(int _no);
    void ReqToCreateTable();
    void ReqToGetReady(bool _ready);
    void ReqToBatCash(game_msg_action_em _act, int _iCash);
private:
    explicit game_control(QObject *parent = nullptr);
    static game_control *m_inst;
    QList<refresh_view*> m_view_list;
    QString m_ssid;
    QWebSocket m_ws;
    QTimer m_heart_timer;
    QTimer m_refresh_timer;
private slots:
    void proc_game_connect();
    void proc_game_disconnect();
    void proc_error(QAbstractSocket::SocketError _err);
    void proc_recv_msg(const QByteArray &_msg);
    void proc_heart_timeout();
    void proc_refresh_timeout();
signals:



    // Imsg_processor interface
public:
    void proc_msg(game_msg_table_info &_msg, char *ssid);
    void proc_msg(game_msg_hand_card_info &_msg, char *ssid);

    // Imsg_processor interface
public:
    void proc_msg(game_msg_last_result &_msg, char *ssid);
};

#endif // GAME_CONTROL_H

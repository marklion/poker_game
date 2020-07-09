#ifndef GAME_MODULE_H
#define GAME_MODULE_H

#include <QObject>
#include <QString>
#include "msg-encode-utils/msg_encode_utils.h"

class game_module : public QObject
{
    Q_OBJECT
public:
    explicit game_module(QObject *parent = nullptr);
    int GetCurTableNo();
    void SyncTableInfo(game_msg_table_info &_msg);
    void UpdateMyself(game_msg_hand_card_info &_msg);
    void UpdateResult(game_msg_last_result &_msg);
    game_msg_player_info *GetPlayerBySeatpos(int _seatpos);
    int GetMySeat();
    int GetMyCurCash();
    int GetMybatCash();
    game_msg_card *GetMyhandCardbyIndex(int _i)
    {
        return &m_my_handcards[_i];
    }
    int GetPoolCash();
    game_msg_card *GetPubCardByIndex(int _i);
    QString GetSelfName();
    bool IsGameStarted();
    int GetActionPos();
    game_msg_single_result *GetsingleResult(int _seat);
    int GetMinBatCash();
signals:

private:
    game_msg_table_info m_curInfo;
    game_msg_card m_my_handcards[2];
    int m_mySeat = 0;
    game_msg_last_result m_result;
};

#endif // GAME_MODULE_H

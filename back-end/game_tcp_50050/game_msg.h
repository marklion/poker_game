#if !defined(_GAME_MSG_H_)
#define _GAME_MSG_H_

#include "pk_game.pb.h"
#include <iostream>
#include <ZinxTCP.h>
#include <zinx.h>

class game_msg
{
public:
    ZinxTcpData *m_from_channel = nullptr;
    google::protobuf::Message *m_pMsg = nullptr;
    enum msg_type
    {
        game_msg_invalid,
        game_msg_sync_ssid,
        game_msg_create_room,
        game_msg_max
    } m_type;
    game_msg(int _iType, google::protobuf::Message *_pMsg):m_pMsg(_pMsg), m_type((msg_type)_iType) {}
    game_msg(int _iType, int _iLen, const char *_pcValue, ZinxTcpData *_from_channel):
        m_from_channel(_from_channel), m_type((msg_type)_iType) {
        switch (m_type)
        {
        case game_msg_sync_ssid:
            m_pMsg = new pk_game::sync_info();
            break;
        case game_msg_create_room:
            m_pMsg = new pk_game::create_room();
            break;
        default:
            break;
        }
        m_pMsg->ParseFromArray(_pcValue, _iLen);
    }
    ~game_msg() {
        if (nullptr != m_pMsg)
        {
            delete m_pMsg;
        }
    }
};

#endif // _GAME_MSG_H_

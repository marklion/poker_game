#include "game_msg_router.h"
#include "pk_game.pb.h"

game_msg_router game_msg_router::gm;

typedef bool (*pfMsgProcessor)(const game_msg *_pMsg);

bool proc_sync_ssid(const game_msg *_pMsg)
{
    bool bRet = false;
    auto pSyncSsid = dynamic_cast<const pk_game::sync_info *>(_pMsg->m_pMsg);

    if (nullptr != pSyncSsid)
    {
        bRet = game_msg_router::GetRouter().record_ssid_channel(pSyncSsid->ssid(), _pMsg->m_from_channel);
    }

    return bRet;
}

bool proc_create_room(const game_msg *pMsg)
{
    bool bRet = false;

    auto create_room_msg = dynamic_cast<const pk_game::create_room *>(pMsg->m_pMsg);
    if (nullptr != create_room_msg)
    {
        auto pRoom = new game_room();
        game_msg_router::GetRouter().add_room(pRoom);

        auto create_room_reply = new pk_game::create_room();
        create_room_reply->set_room_no(pRoom->GetRoomNo());

        game_msg msg(game_msg::game_msg_create_room, create_room_reply);
        game_msg_router::GetRouter().send_msg_to_ssid(create_room_msg->ssid(), &msg);

        bRet = true;
    }

    return bRet;
}

static pfMsgProcessor aMsgProcMap[] = {
    [game_msg::game_msg_invalid] = nullptr,
    [game_msg::game_msg_sync_ssid] = proc_sync_ssid,
    [game_msg::game_msg_create_room] = proc_create_room,
    [game_msg::game_msg_max] = nullptr,
};

bool game_msg_router::record_ssid_channel(const std::string &_ssid, ZinxTcpData *_channel)
{
    bool bRet = false;
    if (m_ssid_channel_map.find(_ssid) == m_ssid_channel_map.end())
    {
        m_ssid_channel_map[_ssid] = _channel;
        m_channel_ssid_map[_channel] = _ssid;
        bRet = true;
    }
    else
    {
        if (m_ssid_channel_map[_ssid] == _channel)
        {
            bRet = true;
        }
        else
        {
            auto orig_channel = m_ssid_channel_map[_ssid];
            m_channel_ssid_map.erase(orig_channel);
            ZinxKernel::Zinx_Del_Channel(*orig_channel);
            delete orig_channel;
            m_ssid_channel_map[_ssid] = _channel;
            m_channel_ssid_map[_channel] = _ssid;
            bRet = true;
        }
    }

    return bRet;
}
void game_msg_router::remove_ssid_channel(const std::string &_ssid)
{
    auto orig_channel = m_ssid_channel_map[_ssid];
    m_channel_ssid_map.erase(orig_channel);
    m_ssid_channel_map.erase(_ssid);
}

void game_msg_router::remove_ssid_channel(ZinxTcpData *_pchannel)
{
    auto orig_ssid = m_channel_ssid_map[_pchannel];
    m_ssid_channel_map.erase(orig_ssid);
    m_channel_ssid_map.erase(_pchannel);
}
void game_msg_router::ProcMsgEntry(const game_msg *_pMsg)
{
    aMsgProcMap[_pMsg->m_type](_pMsg);

    return;
}

bool game_msg_router::send_msg_to_ssid(const std::string &_ssid, const game_msg *_pMsg)
{
    bool bRet = false;

    auto pChannel = m_ssid_channel_map[_ssid];
    if (nullptr != pChannel)
    {
        int iType = htonl(_pMsg->m_type);
        std::string szValue;
        _pMsg->m_pMsg->SerializeToString(&szValue);
        int iLen = htonl(szValue.length());

        std::string szOut;
        szOut.append((const char *)&iType, sizeof(iType));
        szOut.append((const char *)&iLen, sizeof(iLen));
        szOut.append(szValue);

        ZinxKernel::Zinx_SendOut(szOut, *pChannel);
        bRet = true;
    }

    return bRet;
}

void game_msg_router::add_room(game_room *_pRoom)
{
    _pRoom->SetRoomNo(m_room_vec.size());
    m_room_vec.push_back(_pRoom);
}
void game_msg_router::del_room(int _room_no)
{
    m_room_vec.erase(m_room_vec.begin() + _room_no);
}
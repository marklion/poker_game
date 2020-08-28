#if !defined(_GAME_MSG_ROUTER_H_)
#define _GAME_MSG_ROUTER_H_

#include "game_msg.h"
#include <map>
#include <zinx.h>
#include <ZinxTCP.h>
#include "game_room.h"
#include <vector>

class game_msg_router
{
private:
    static game_msg_router gm;
    game_msg_router(){}
    ~game_msg_router(){}
    std::map<std::string,ZinxTcpData *> m_ssid_channel_map;
    std::map<ZinxTcpData *, std::string> m_channel_ssid_map;
    std::vector<game_room *> m_room_vec;
public:
    static game_msg_router &GetRouter() {
        return gm;
    }
    void ProcMsgEntry(const game_msg *_pMsg);
    bool record_ssid_channel(const std::string &_ssid, ZinxTcpData *_channel);
    void remove_ssid_channel(const std::string &_ssid);
    void remove_ssid_channel(ZinxTcpData *_pchannel);
    bool send_msg_to_ssid(const std::string &_ssid, const game_msg *_pMsg);
    void add_room(game_room *_pRoom);
    void del_room(int _room_no);
};


#endif // _GAME_MSG_ROUTER_H_

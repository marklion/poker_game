#if !defined(_GAME_MSG_PROCESSOR_H)
#define _GAME_MSG_PROCESSOR_H
#include <zinx.h>
#include <cstdio>
#include <ZinxTCP.h>
#include <map>
#include "../../front-end/poker_game/msg-encode-utils/msg_encode_utils.h"
#include "game_table.h"
#include <vector>
#include "game_table.h"

class game_msg_processor:public AZinxHandler, public Imsg_processor{
	virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput);
	virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg);
    game_msg_processor(){}
    static game_msg_processor *pinst;
    std::map<std::string,ZinxTcpData *> m_ssid_channel_map;

    ZinxTcpData *GetChannelBySSID(const char *_ssid);

    std::map<std::string,game_player*> m_ssid_player_map;
    game_player *FetchPlayer(std::string ssid);
    std::map<int, game_table *> m_table_no_map;
    game_table *create_table();
public:
    game_table *get_table(int _no);
    static game_msg_processor *GetInst() {
        if (NULL == pinst)
        {
            pinst = new game_msg_processor();
        }
        return pinst;
    }
    void SetChannelBySSID(const char *_ssid, ZinxTcpData *_pChannel)
    {
        std::string ssid_key(_ssid, GAME_MSG_SSID_LEN);
        m_ssid_channel_map[ssid_key] = _pChannel;
    }

    void ClearChannelBySSID(const char *_ssid)
    {
        std::string ssid_key(_ssid, GAME_MSG_SSID_LEN);
        m_ssid_channel_map.erase(ssid_key);
    }
    void SyncTableInfo(int _no);
    void SyncPlayerInfo(std::string _ssid);
    void SyncLastResult(int _no);
    virtual void proc_msg(game_msg_enter_table &_msg, char *ssid);
    virtual void proc_msg(game_msg_create_table &_msg, char *ssid);
    virtual void proc_msg(game_msg_ready &_msg, char *ssid);
    virtual void proc_msg(game_msg_action &_msg, char *ssid);
    void KickOffPlayer(std::string _ssid);
};



#endif // _GAME_MSG_PROCESSOR_H

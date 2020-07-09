#include "msg_encode_utils.h"
#include <string.h>

static int g_aiTypeLenMap[] = {
    [game_msg_type_invalid] = 0,
    [game_msg_type_enter_table] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_enter_table),
    [game_msg_type_table_info] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_table_info),
    [game_msg_type_handcard_info] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_hand_card_info),
    [game_msg_type_create_table] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_create_table),
    [game_msg_type_heart] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_heart),
    [game_msg_type_ready] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_ready),
    [game_msg_type_action] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_action),
    [game_msg_type_last_result] = MSG_GET_TOTAL_LEN_BY_TYPE(game_msg_last_result),
    [game_msg_type_max] = 0};

int msg_GetBuffLen(game_msg_type_em _enType)
{
    return g_aiTypeLenMap[_enType];
}
game_msg_type_em msg_GetType(const char *_pcBuff, int _iLen)
{
    game_msg_type_em enRet = game_msg_type_invalid;

    if ((unsigned int)_iLen >= MSG_MIN_LEN())
    {
        memcpy(&enRet, _pcBuff + GAME_MSG_SSID_LEN, sizeof(enRet));
        enRet = (game_msg_type_em)ntohl(enRet);
    }

    return enRet;
}

int MSG_GetMsgLenFrombuff(const char *pcBuff, int _iLen)
{
    int iRet = -1;

    if ((unsigned int)_iLen >= MSG_MIN_LEN())
    {
        memcpy(&iRet, pcBuff + GAME_MSG_SSID_LEN + sizeof(game_msg_type_em), sizeof(int));
        iRet = ntohl(iRet);
    }

    return iRet;
}

bool MSG_GetSSIDFromBuff(const char *pcBuff, int _iLen, char *_pcSsid)
{
    bool bRet = false;

    if (_iLen >= GAME_MSG_SSID_LEN)
    {
        memcpy(_pcSsid, pcBuff, GAME_MSG_SSID_LEN);
        bRet = true;
    }

    return bRet;
}

void recv_and_proccess_msg(const char *_pcBuff, int _iBuffLen, Imsg_processor *_processor)
{
    _processor->tmp.append(_pcBuff, _iBuffLen);
    char ssid[GAME_MSG_SSID_LEN];

    auto &cur_buff_st = _processor->tmp;

    while (cur_buff_st.length() >= (MSG_GetMsgLenFrombuff(cur_buff_st.data(), cur_buff_st.length()) + sizeof(int) + sizeof(game_msg_type_em) + GAME_MSG_SSID_LEN))
    {
        int buff_msg_len = MSG_GetMsgLenFrombuff(cur_buff_st.data(), cur_buff_st.length());
        if (buff_msg_len < 0)
        {
            return;
        }
        const char *cur_pos = cur_buff_st.data();
        int cur_len = buff_msg_len + sizeof(int) + sizeof(game_msg_type_em) + GAME_MSG_SSID_LEN;
        game_msg_type_em enType = msg_GetType(cur_pos, cur_len);

        switch (enType)
        {
        case game_msg_type_enter_table:
        {
            game_msg_enter_table msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        case game_msg_type_handcard_info:
        {
            game_msg_hand_card_info msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        case game_msg_type_table_info:
        {
            game_msg_table_info msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        case game_msg_type_create_table:
        {
            game_msg_create_table msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        case game_msg_type_ready:
        {
            game_msg_ready msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        case game_msg_type_action:
        {
            game_msg_action msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        case game_msg_type_last_result:
        {
            game_msg_last_result msg;
            MSG_DECODE(cur_pos, cur_len, msg, ssid);
            _processor->proc_msg(msg, ssid);
            break;
        }
        default:
            break;
        }
        cur_buff_st.erase(0, cur_len);
    }
}

#include "game_msg_processor.h"
#include <hiredis/hiredis.h>

game_msg_processor *game_msg_processor::pinst = NULL;

IZinxMsg *game_msg_processor::InternelHandle(IZinxMsg &_oInput)
{
    GET_REF2DATA(BytesMsg, bytemsg, _oInput);
    const char *pcMsgBuff = bytemsg.szData.data();
    int iMsgLen = bytemsg.szData.length();

    recv_and_proccess_msg(pcMsgBuff, iMsgLen, this);

    return NULL;
}

AZinxHandler *game_msg_processor::GetNextHandler(IZinxMsg &_oNextMsg)
{
    return NULL;
}

ZinxTcpData *game_msg_processor::GetChannelBySSID(const char *_ssid)
{
    std::string ssid_key(_ssid, GAME_MSG_SSID_LEN);

    return m_ssid_channel_map[ssid_key];
}

void game_msg_processor::proc_msg(game_msg_enter_table &_msg, char *ssid)
{
    int iReqTableNo = _msg.iTableId;
    auto ptable = get_table(iReqTableNo);
    if (nullptr != ptable)
    {
        auto pPlayer = FetchPlayer(std::string(ssid, GAME_MSG_SSID_LEN));

        if (true == ptable->AddPlayer(pPlayer))
        {
            SyncPlayerInfo(std::string(std::string(ssid, GAME_MSG_SSID_LEN)));
            SyncTableInfo(ptable->m_table_no);
        }
    }
}

void game_msg_processor::proc_msg(game_msg_create_table &_msg, char *ssid)
{
    auto ptable = create_table();

    auto pPlayer = FetchPlayer(std::string(ssid, GAME_MSG_SSID_LEN));
    ptable->AddPlayer(0, pPlayer);

    SyncPlayerInfo(std::string(ssid, GAME_MSG_SSID_LEN));
    SyncTableInfo(ptable->m_table_no);
}

void game_msg_processor::SyncTableInfo(int _no)
{
    auto pTable = get_table(_no);
    if (nullptr == pTable)
        return;
    game_msg_table_info msg;
    msg.iCurBatCash = pTable->GetCurBatCash();
    msg.iCurPos = pTable->GetCurDealerPos();
    msg.iTableNo = _no;
    msg.iActionPos = pTable->GetActionPos();
    msg.iMinBatCash = pTable->GetMinBatCash();
    if (pTable->IsStarted())
    {
        msg.enState = game_msg_game_state_start;
    }
    else
    {
        msg.enState = game_msg_game_state_ready;
    }


    for (int i = 0; i < 6; i++)
    {
        auto player = pTable->GetPlayerBySeat(i);
        if (NULL != player)
        {
            msg.astAllPlayer[i].ibatCash = player->GetBatCash();
            msg.astAllPlayer[i].icurrentCash = player->GetCurCash();
            msg.astAllPlayer[i].iSeatPos = i;
            msg.astAllPlayer[i].bReady = player->IsReady();
            msg.astAllPlayer[i].bFalled = player->IsFalled();
            msg.astAllPlayer[i].bAllined = player->IsAllin();
            strncpy(msg.astAllPlayer[i].szName, player->GetName().c_str(), GAME_MSG_PLAYER_NAME_MAXLEN);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        auto pcard = pTable->GetTableCard(i);
        if (pcard->m_color != single_card::invalid)
        {
            msg.astTableCards[i].number = pcard->m_num;
            msg.astTableCards[i].color = (game_msg_card_color)(pcard->m_color);
        }
    }

    int iSendLen = msg_GetBuffLen(game_msg_type_table_info);
    char *pcBuff = (char *)calloc(1UL, iSendLen);

    for (int i = 0; i < 6; i++)
    {
        auto pplayer = pTable->GetPlayerBySeat(i);
        if (nullptr != pplayer)
        {
            auto ssid = pplayer->GetSSID();
            MSG_ENCODE(msg, ssid.data(), game_msg_type_table_info, pcBuff);
            std::string outdata(pcBuff, iSendLen);
            auto poutChannel = GetChannelBySSID(ssid.data());
            if (nullptr != poutChannel)
                ZinxKernel::Zinx_SendOut(outdata, *poutChannel);
        }
    }
    free(pcBuff);
}
void game_msg_processor::SyncPlayerInfo(std::string _ssid)
{
    auto pplayer = m_ssid_player_map[_ssid];
    if (nullptr == pplayer)
        return;
    game_msg_hand_card_info msg;

    msg.iSelfSeat = pplayer->m_iSeatPos;
    msg.hand_card[0].color = (game_msg_card_color)pplayer->GetHandCard(0)->m_color;
    msg.hand_card[0].number = pplayer->GetHandCard(0)->m_num;
    msg.hand_card[1].color = (game_msg_card_color)pplayer->GetHandCard(1)->m_color;
    msg.hand_card[1].number = pplayer->GetHandCard(1)->m_num;

    int SendLen = msg_GetBuffLen(game_msg_type_handcard_info);
    char *pcBuff = (char *)calloc(1UL, SendLen);
    MSG_ENCODE(msg, _ssid.data(), game_msg_type_handcard_info, pcBuff);
    std::string outdata(pcBuff, SendLen);
    free(pcBuff);
    auto poutChannel = GetChannelBySSID(_ssid.data());
    if (nullptr != poutChannel)
        ZinxKernel::Zinx_SendOut(outdata, *poutChannel);
}

void game_msg_processor::SyncLastResult(int _no)
{
    auto pTable = get_table(_no);
    if (nullptr == pTable)
        return;

    game_msg_last_result msg;
    for (int i = 0; i < 6; i++)
    {
        auto itr = pTable->GetPlayerBySeat(i);
        if (nullptr != itr)
        {
            game_msg_single_result submsg;
            submsg.bShow = !(itr->IsFalled());
            if (pTable->GetNumberOnTable() <= 1)
            {
                submsg.bShow = false;
            }
            if (!(itr->IsFalled()))
            {
                submsg.enType = (game_msg_pattern_type)(itr->GetPattern().GetType());
                for (int i = 0; i < 5; i++)
                {
                    submsg.finalPattern[i].color = (game_msg_card_color)(itr->GetPattern().GetFinalCard(i).m_color);
                    submsg.finalPattern[i].number = itr->GetPattern().GetFinalCard(i).m_num;
                }
            }
            msg.allresult[itr->m_iSeatPos] = submsg;
        }
    }

    int iSendLen = msg_GetBuffLen(game_msg_type_last_result);
    char *pcBuff = (char *)calloc(1UL, iSendLen);

    for (int i = 0; i < 6; i++)
    {
        auto pplayer = pTable->GetPlayerBySeat(i);
        if (nullptr != pplayer)
        {
            auto ssid = pplayer->GetSSID();
            MSG_ENCODE(msg, ssid.data(), game_msg_type_last_result, pcBuff);
            std::string outdata(pcBuff, iSendLen);
            auto poutChannel = GetChannelBySSID(ssid.data());
            if (nullptr != poutChannel)
                ZinxKernel::Zinx_SendOut(outdata, *poutChannel);
        }
    }
    free(pcBuff);
}

void game_msg_processor::proc_msg(game_msg_ready &_msg, char *_ssid)
{
    std::string ssid(_ssid, GAME_MSG_SSID_LEN);
    auto pplayer = m_ssid_player_map[ssid];
    if (nullptr != pplayer)
    {
        if (_msg.bReady != 0)
        {
            pplayer->SetReady(true);
        }
        else
        {
            pplayer->SetReady(false);
        }
        auto ptable = get_table(pplayer->GetTable());
        if (nullptr != ptable)
            ptable->Start();
        SyncTableInfo(pplayer->GetTable());
    }
}

void game_msg_processor::proc_msg(game_msg_action &_msg, char *_ssid)
{
    action_in_table::action_type action_map[] = {
        [game_msg_action_call] = action_in_table::call,
        [game_msg_action_bat] = action_in_table::bat,
        [game_msg_action_raise] = action_in_table::raise,
        [game_msg_action_check] = action_in_table::check,
        [game_msg_action_fall] = action_in_table::fall,
        [game_msg_action_allin] = action_in_table::allin,
        [game_msg_action_max] = action_in_table::invalid};
    std::string ssid(_ssid, GAME_MSG_SSID_LEN);
    auto pplayer = m_ssid_player_map[ssid];
    if (nullptr != pplayer)
    {
        auto ptable = get_table(pplayer->GetTable());
        if (nullptr != ptable)
        {
            action_in_table action;
            action.action = action_map[_msg.enAction];
            action.action_no = pplayer->m_iSeatPos;
            action.cash = _msg.iCash;
            ptable->ProcAction(action);
        }
    }
}
game_player *game_msg_processor::FetchPlayer(std::string ssid)
{
    if (m_ssid_player_map[ssid] == nullptr)
    {
        auto predis = redisConnect("localhost", 6379);
        auto preply = (redisReply *)redisCommand(predis, "HGET user_ssid:%s name", ssid.c_str());
        std::string user_name(preply->str, preply->len);
        freeReplyObject(preply);

        preply = (redisReply *)redisCommand(predis, "HGET user_ssid:%s cash", ssid.c_str());
        int user_cash = atoi(preply->str);
        redisFree(predis);
        auto pPlayer = new game_player(user_name, ssid, user_cash);
        m_ssid_player_map[ssid] = pPlayer;
    }

    return m_ssid_player_map[ssid];
}

game_table *game_msg_processor::create_table()
{
    game_table *pret = nullptr;
    int i = 0;
    while (true)
    {
        if (NULL == m_table_no_map[i])
        {
            pret = new game_table(i);
            m_table_no_map[i] = pret;
            break;
        }
        i++;
    }

    return pret;
}
game_table *game_msg_processor::get_table(int _no)
{
    return m_table_no_map[_no];
}

void game_msg_processor::KickOffPlayer(std::string _ssid)
{
    auto pchannel = GetChannelBySSID(_ssid.data());

    ZinxKernel::Zinx_Del_Channel(*pchannel);
    delete pchannel;
}
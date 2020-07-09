#include "game_module.h"

game_module::game_module(QObject *parent) : QObject(parent)
{

}

int game_module::GetCurTableNo()
{
    return m_curInfo.iTableNo;
}

void game_module::SyncTableInfo(game_msg_table_info &_msg)
{
    memcpy(&m_curInfo, &_msg, sizeof(m_curInfo));
}

void game_module::UpdateMyself(game_msg_hand_card_info &_msg)
{
    m_mySeat = _msg.iSelfSeat;
    memcpy(m_my_handcards, _msg.hand_card, sizeof(m_my_handcards));
}

void game_module::UpdateResult(game_msg_last_result &_msg)
{
    memcpy(&m_result, &_msg, sizeof(m_result));
}

game_msg_player_info *game_module::GetPlayerBySeatpos(int _seatpos)
{
    game_msg_player_info *pret = NULL;

    for (auto &player_itr:m_curInfo.astAllPlayer)
    {
        if (_seatpos == player_itr.iSeatPos)
        {
             pret = &player_itr;
             break;
        }
    }

    return pret;
}

int game_module::GetMySeat()
{
    return m_mySeat;
}

int game_module::GetMyCurCash()
{
    return m_curInfo.astAllPlayer[m_mySeat].icurrentCash;
}

int game_module::GetMybatCash()
{
    return m_curInfo.astAllPlayer[m_mySeat].ibatCash;
}

int game_module::GetPoolCash()
{
    int ret = 0;
    ret = m_curInfo.iCurBatCash;

    return ret;
}

game_msg_card *game_module::GetPubCardByIndex(int _i)
{
    if (m_curInfo.astTableCards[_i].color != game_card_invalid)
        return &m_curInfo.astTableCards[_i];

    return nullptr;
}

QString game_module::GetSelfName()
{
    return QString(m_curInfo.astAllPlayer[m_mySeat].szName);
}

bool game_module::IsGameStarted()
{
    if (m_curInfo.enState == game_msg_game_state_start)
    {
        return true;
    }

    return false;
}

int game_module::GetActionPos()
{
    int iRet = -1;

    iRet = m_curInfo.iActionPos;

    return iRet;
}

game_msg_single_result *game_module::GetsingleResult(int _seat)
{
    return &m_result.allresult[_seat];
}

int game_module::GetMinBatCash()
{
    return m_curInfo.iMinBatCash;
}


#include "game_table.h"
#include <random>
#include <fstream>
#include <time.h>
#include "game_msg_processor.h"
#include <algorithm>
#include "timer_channel.h"

void game_table::Start()
{
    int iReadyNum = 0;
    int iTotalNum = 0;

    for (auto itr:m_apxPlayer)
    {
        if (nullptr != itr && itr->GetCurCash() <= 20)
        {
            game_msg_processor::GetInst()->KickOffPlayer(itr->GetSSID());
            DelPlayer(itr->m_iSeatPos);
            SetStartState(false);
            InitDealerPos();
        }
    }

    for (auto itr:m_apxPlayer)
    {
        if (nullptr != itr)
        {
            iTotalNum++;
            if (itr->IsReady())
            {
                iReadyNum++;
            }
        }
    }

    if (iReadyNum == iTotalNum && iTotalNum >= 2)
    {
        SetStartState(true);
        AbsState *pNextStat = m_pCurState->procStart(this);

        while (pNextStat != m_pCurState)
        {
            delete m_pCurState;
            m_pCurState = pNextStat;
            pNextStat = m_pCurState->work(this);
        }
    }
}

void game_table::Stop()
{
    AbsState *pNextStat = m_pCurState->procStop(this);

    while (pNextStat != m_pCurState)
    {
        delete m_pCurState;
        m_pCurState = pNextStat;
        pNextStat = m_pCurState->work(this);
    }
}

int game_table::GetNumberOnTable()
{
    int valid_num = 0;

    for (auto itr:m_apxPlayer)
    {
        if (nullptr != itr && !(itr->IsFalled()))
        {
            valid_num++;
        }
    }

    return valid_num;

}
void game_table::ProcAction(action_in_table &_action)
{
    timeout_deliver::GetInstance()->unregister_task(GetPlayerBySeat(_action.action_no));
    AbsState *pNextStat = m_pCurState->procAction(this, _action);

    while (pNextStat != m_pCurState)
    {
        delete m_pCurState;
        m_pCurState = pNextStat;
        pNextStat = m_pCurState->work(this);
    }
    game_msg_processor::GetInst()->SyncTableInfo(m_table_no);
}

bool game_table::AddPlayer(int _seat, game_player *_player)
{
    bool bRet = false;

    if (false == IsStarted())
    {
        if (nullptr == m_apxPlayer[_seat])
        {
            m_apxPlayer[_seat] = _player;
            _player->m_iSeatPos = _seat;
            _player->SetTable(this->m_table_no);
            bRet = true;
        }
    }

    return bRet;
}
bool game_table::AddPlayer(game_player *_player)
{
    bool bRet = false;
    bool bExist = false;
    for (auto itr : m_apxPlayer)
    {
        if (itr == _player)
        {
            bExist = true;
            break;
        }
    }

    if (false == bExist)
    {
        for (int i = 0; i < 6; i++)
        {
            if (nullptr == m_apxPlayer[i])
            {
                bRet = AddPlayer(i, _player);
                break;
            }
        }
    }

    return bRet;
}

void game_table::DelPlayer(int _seat)
{
    m_apxPlayer[_seat] = nullptr;
}
game_player *game_table::GetPlayerBySeat(int _seat)
{
    return m_apxPlayer[_seat];
}

game_player::game_player(std::string _name, std::string _ssid, int _cash) : m_szName(_name), m_iCash(_cash), m_ssid(_ssid)
{
}

bool AbsState::bat_or_continue(game_table *_pgame_table, action_in_table &_action)
{
    bool bFinish = false;

    if (_action.action_no == _pgame_table->GetActionPos())
    {
        auto action_player = _pgame_table->GetPlayerBySeat(_action.action_no);
        int iCashOut = action_player->ExecAction(_action, _pgame_table);

        if (iCashOut < _pgame_table->GetMinBatCash() && 0 != action_player->GetCurCash())
        {
            action_player->SetFalled(true);
        }

        if (0 < iCashOut && iCashOut >= _pgame_table->GetMinBatCash() * 2)
        {
            _pgame_table->SetMinBatCash(iCashOut);
            _pgame_table->SetLastActionSeat(_pgame_table->GetPrevPlayer(action_player->m_iSeatPos)->m_iSeatPos);
        }
        if (0 == action_player->GetCurCash())
        {
            action_player->SetAllin(true);
        }

        if (_action.action_no == _pgame_table->GetLastActionSeat() || _pgame_table->GetNumberOnTable() <= 1)
        {
            _pgame_table->DeliverPool();
            _pgame_table->SetMinBatCash(0);
            bFinish = true;
        }
        else
        {
            _pgame_table->SetActionPos(_pgame_table->GetNextPlayer(_action.action_no)->m_iSeatPos);
            if (_action.action_no ==  _pgame_table->GetActionPos())
            {
                _pgame_table->DeliverPool();
                _pgame_table->SetMinBatCash(0);
                bFinish = true;
            }
        }
    }

    return bFinish;
}
AbsState *game_state_init::procStart(game_table *_table)
{
    return new game_state_preFlop();
}

AbsState *game_state_preFlop::work(game_table *_pgame_table)
{
    int iCurDealerPos = _pgame_table->GetCurDealerPos();
    auto sbPlayer = _pgame_table->GetNextPlayer(iCurDealerPos);
    auto bbPlayer = _pgame_table->GetNextPlayer(sbPlayer->m_iSeatPos);

    for (int i = 0; i < 5; i++)
    {
        _pgame_table->SetTableCard(i, single_card());
    }


    for (int i = 0; i < 6; i++)
    {
        auto unFall = _pgame_table->GetPlayerBySeat(i);
        if (nullptr != unFall)
        {
            unFall->ResetDefault();
        }
    }


    sbPlayer->BatCashOut(_pgame_table->GetSmallBlind(), _pgame_table);
    bbPlayer->BatCashOut(_pgame_table->GetSmallBlind() * 2, _pgame_table);
    _pgame_table->SetMinBatCash(bbPlayer->GetBatCash());

    single_card tmp[2];
    tmp[0] = _pgame_table->CardPop();
    tmp[1] = _pgame_table->CardPop();
    _pgame_table->GetPlayerBySeat(iCurDealerPos)->SetHandCards(tmp);
    auto card_div_player = _pgame_table->GetNextPlayer(iCurDealerPos);
    while (card_div_player->m_iSeatPos != iCurDealerPos)
    {
        single_card tmp[2];
        tmp[0] = _pgame_table->CardPop();
        tmp[1] = _pgame_table->CardPop();
        card_div_player->SetHandCards(tmp);
        card_div_player = _pgame_table->GetNextPlayer(card_div_player->m_iSeatPos);
    }
    _pgame_table->SetActionPos(_pgame_table->GetNextPlayer(bbPlayer->m_iSeatPos)->m_iSeatPos);
    _pgame_table->SetLastActionSeat(bbPlayer->m_iSeatPos);

    return this;
}
AbsState *game_state_preFlop::procAction(game_table *_pgame_table, action_in_table &_action)
{
    AbsState *pRet = this;

    if (bat_or_continue(_pgame_table, _action))
    {
        pRet = new game_state_Flop();
    }

    return pRet;
}

AbsState *game_state_Flop::work(game_table *_pgame_table)
{
    _pgame_table->SetTableCard(0, _pgame_table->CardPop());
    _pgame_table->SetTableCard(1, _pgame_table->CardPop());
    _pgame_table->SetTableCard(2, _pgame_table->CardPop());


    if (_pgame_table->GetValidNum() < 2)
    {
        return new game_state_Turn();
    }

    _pgame_table->SetLastActionSeat(_pgame_table->GetCurDealerPos());
    _pgame_table->SetActionPos(_pgame_table->GetNextPlayer(_pgame_table->GetCurDealerPos())->m_iSeatPos);
    return this;
}
AbsState *game_state_Flop::procAction(game_table *_pgame_table, action_in_table &_action)
{
    AbsState *pRet = this;

    if (bat_or_continue(_pgame_table, _action))
    {
        pRet = new game_state_Turn();
    }

    return pRet;
}

AbsState *game_state_Turn::work(game_table *_pgame_table) {
    _pgame_table->SetTableCard(3, _pgame_table->CardPop());

    if (_pgame_table->GetValidNum() < 2)
    {
        return new game_state_River();
    }

    _pgame_table->SetLastActionSeat(_pgame_table->GetCurDealerPos());
    _pgame_table->SetActionPos(_pgame_table->GetNextPlayer(_pgame_table->GetCurDealerPos())->m_iSeatPos);
    return this;
}

AbsState *game_state_Turn::procAction(game_table *_pgame_table, action_in_table &_action)
{
    AbsState *pRet = this;

    if (bat_or_continue(_pgame_table, _action))
    {
        pRet = new game_state_River();
    }

    return pRet;
}

AbsState *game_state_River::work(game_table *_pgame_table)
{
    _pgame_table->SetTableCard(4, _pgame_table->CardPop());

    if (_pgame_table->GetValidNum() < 2)
    {
        return new game_state_Finish();
    }

    _pgame_table->SetLastActionSeat(_pgame_table->GetCurDealerPos());
    _pgame_table->SetActionPos(_pgame_table->GetNextPlayer(_pgame_table->GetCurDealerPos())->m_iSeatPos);
    return this;
}

AbsState *game_state_River::procAction(game_table *_pgame_table, action_in_table &_action)
{
    AbsState *pRet = this;

    if (bat_or_continue(_pgame_table, _action))
    {
        pRet = new game_state_Finish();
    }

    return pRet;
}

AbsState *game_state_Finish::work(game_table *_pgame_table)
{
    std::vector<game_player *> alive_players;
    for (int i = 0; i < 6; i++)
    {
        single_card on_table_cards[7];
        for (int i = 0; i < 5; i++)
        {
            on_table_cards[i] = *(_pgame_table->GetTableCard(i));
        }
        auto player = _pgame_table->GetPlayerBySeat(i);
        if (nullptr != player && false == player->IsFalled())
        {
            on_table_cards[5] = *(player->GetHandCard(0));
            on_table_cards[6] = *(player->GetHandCard(1));
            player->SetPattern(card_pattern::MakeFinalPattern(on_table_cards));
            alive_players.push_back(player);
        }
    }

    std::sort(alive_players.begin(), alive_players.end(), [](game_player *a, game_player *b) {
        return a->GetPattern() > b->GetPattern();
    });

    for (auto itr:alive_players)
    {
        itr->IncreatseCash(_pgame_table->GetSidePool(itr->m_iSeatPos));
        _pgame_table->DecreasePool(_pgame_table->GetSidePool(itr->m_iSeatPos));
    }

    _pgame_table->showout_result();

    timeout_deliver::GetInstance()->register_task(10, _pgame_table);
    _pgame_table->ResetAllPlayer();
    _pgame_table->ResetHeap();

    _pgame_table->MoveDealer();
    _pgame_table->ResetActionPos();

    return new game_state_init();
}

cards_heap::cards_heap()
{
    single_card::color_em tmp_color;

    tmp_color = single_card::heart;
    for (int i = 1; i <= 13; i++)
    {
        single_card tmp_card(tmp_color, i);
        m_cards.push_back(tmp_card);
    }
    tmp_color = single_card::spade;
    for (int i = 1; i <= 13; i++)
    {
        single_card tmp_card(tmp_color, i);
        m_cards.push_back(tmp_card);
    }
    tmp_color = single_card::diamond;
    for (int i = 1; i <= 13; i++)
    {
        single_card tmp_card(tmp_color, i);
        m_cards.push_back(tmp_card);
    }
    tmp_color = single_card::club;
    for (int i = 1; i <= 13; i++)
    {
        single_card tmp_card(tmp_color, i);
        m_cards.push_back(tmp_card);
    }

}

static std::default_random_engine e(time(NULL));
single_card cards_heap::PopOneCard()
{
    int total_num = m_cards.size();
    int numb = e() % total_num;

    single_card ret = m_cards[numb];
    m_cards.erase(m_cards.begin() + numb);

    return ret;
}
void game_table::InitDealerPos()
{
    m_curDealer_pos = 0;
    for (auto itr:m_apxPlayer)
    {
        if (nullptr != itr)
        {
            m_curDealer_pos = itr->m_iSeatPos;
            break;
        }
    }
}
void game_table::MoveDealer()
{
    m_curDealer_pos = GetNextPlayer(m_curDealer_pos)->m_iSeatPos;
}
game_player *game_table::GetNextPlayer(int _seat)
{
    int iNext = (_seat + 1) % 6;
    while (nullptr == m_apxPlayer[iNext])
    {
        iNext++;
        iNext %=6;
    }

    if (m_apxPlayer[iNext]->IsFalled())
    {
        return GetNextPlayer(m_apxPlayer[iNext]->m_iSeatPos);
    }
    else
    {
        return m_apxPlayer[iNext];
    }
}

game_player *game_table::GetPrevPlayer(int _seat)
{
    int iPrev = (_seat + 5) % 6;
    while (nullptr == m_apxPlayer[iPrev])
    {
        iPrev += 5;
        iPrev %=6;
    }

    if (m_apxPlayer[iPrev]->IsFalled())
    {
        return GetPrevPlayer(m_apxPlayer[iPrev]->m_iSeatPos);
    }
    else
    {
        return m_apxPlayer[iPrev];
    }
}

void game_table::DeliverPool()
{
    for (auto itr:m_apxPlayer)
    {
        if (nullptr != itr && false == itr->IsFalled())
        {
            int ibatCash = itr->GetBatCash();
            if (ibatCash <= 0)
            {
                continue;
            }
            for (auto other:m_apxPlayer)
            {
                if (nullptr != other && other->GetBatCash() <= ibatCash)
                {
                    m_side_pool[itr->m_iSeatPos] += other->GetBatCash();
                }
                else if (nullptr != other && other->GetBatCash() > ibatCash)
                {
                    m_side_pool[itr->m_iSeatPos] += ibatCash;
                }
            }
        }
    }
    for (auto itr:m_apxPlayer) {
        if (nullptr != itr)
        {
            itr->SetBatCash(0);
        }
    }
}

int game_player::BatCashOut(int _cash, game_table *_ptable)
{
    _ptable->MainPoolinCash(0 - m_iBatCash);
    m_iCash += m_iBatCash;
    if (_cash > this->m_iCash)
    {
        m_iBatCash = m_iCash;
    }
    else
    {
        m_iBatCash = _cash;
    }

    m_iCash -= m_iBatCash;
    _ptable->MainPoolinCash(m_iBatCash);

    return m_iBatCash;
}
int game_table::GetValidNum()
{
    int valid_num = 0;

    for (auto itr:m_apxPlayer)
    {
        if (nullptr != itr && !(itr->IsAllin()) && !(itr->IsFalled()))
        {
            valid_num++;
        }
    }

    return valid_num;
}

void game_table::DecreasePool(int _cash)
{
    m_main_pool -= _cash;
    for (auto &itr:m_side_pool)
    {
        itr -= _cash;
        if (itr <= 0)
        {
            itr = 0;
        }
    }
}

void game_table::showout_result() {
    game_msg_processor::GetInst()->SyncLastResult(this->m_table_no);
}

void game_table::proc_timeout()
{
    timeout_deliver::GetInstance()->unregister_task(this);
    Start();
    game_msg_processor::GetInst()->SyncTableInfo(m_table_no);
}
int game_player::ExecAction(const action_in_table &_action, game_table *_pgame_table)
{
    int iRet = 0;

    switch (_action.action)
    {
    case action_in_table::call:
    case action_in_table::bat:
    case action_in_table::raise:
    case action_in_table::allin:
    {
        iRet = BatCashOut(_action.cash, _pgame_table);
        break;
    }
    case action_in_table::fall:
    {
        SetFalled(true);
        break;
    }

    default:
        break;
    }

    return iRet;
}

void game_player::SyncOut()
{
    game_msg_processor::GetInst()->SyncPlayerInfo(this->m_ssid);
}
void game_player::proc_timeout()
{
    auto pTable = game_msg_processor::GetInst()->get_table(GetTable());
    action_in_table fall_action;
    fall_action.action = action_in_table::fall;
    fall_action.action_no = m_iSeatPos;
    pTable->ProcAction(fall_action);
}

typedef std::vector<single_card> wait_pat;

card_pattern make_straight(const wait_pat &_cards)
{
    if (_cards.size() < 5)
        return card_pattern();

    bool isStraight = true;
    for (int i = 0; i < 4; i++)
    {
        if (_cards[i] - _cards[i+1] != 1)
        {
            isStraight = false;
            break;
        }
    }

    if (isStraight)
    {
        bool isFlush = true;
        single_card::color_em tmp_color = _cards[0].m_color;
        for (int i = 0; i < 5; i++)
        {
            if (tmp_color != _cards[i].m_color)
            {
                isFlush = false;
                break;
            }
        }

        if (isFlush)
        {
            return card_pattern(card_pattern::straight_flush, _cards.data());
        }

        return card_pattern(card_pattern::straight, _cards.data());
    }

    if (_cards[0].m_num != 1)
    {
        wait_pat tmp = _cards;
        tmp.erase(tmp.begin());
        return make_straight(tmp);
    }

    wait_pat tmp = _cards;
    tmp.erase(tmp.begin());
    tmp.push_back(_cards[0]);

    return make_straight(tmp);
}

card_pattern make_flush(const wait_pat &_cards)
{
    std::vector<single_card> card_map_color[single_card::invalid];
    for (auto &itr:_cards)
    {
        card_map_color[itr.m_color].push_back(itr);
    }
    for (auto &itr:card_map_color)
    {
        if (itr.size() >= 5)
        {
            auto tmp = make_straight(itr);
            if(tmp.GetType() == card_pattern::straight_flush)
            {
                return tmp;
            }
            return card_pattern(card_pattern::flush, itr.data());
            break;
        }
    }

    return card_pattern();
}

bool is_same_n(const wait_pat &_orig, int _i, int _n)
{
    bool bRet = true;
    for (int i = _i; i < _n + _i; i++)
    {
        if (_orig[_i] != _orig[i])
        {
            bRet = false;
            break;
        }
    }

    return bRet;
}

card_pattern make_fok(const wait_pat &_cards)
{
    for (int i = 0; i < 4; i++)
    {
        if (is_same_n(_cards,  i, 4))
        {
            std::vector<single_card> tmp = _cards;
            tmp.erase(tmp.begin() + i, tmp.begin() + i + 4);
            tmp.insert(tmp.begin(), _cards.begin() + i, _cards.begin() + i + 4);
            return card_pattern(card_pattern::fok,tmp.data());
        }
    }

    return card_pattern();
}

card_pattern make_fullhouse_or_set(const wait_pat &_cards)
{
    for (int i = 0; i < 5; i++)
    {
        if (is_same_n(_cards, i, 3))
        {
            std::vector<single_card> tmp = _cards;
            tmp.erase(tmp.begin() + i, tmp.begin() + i + 3);
            tmp.insert(tmp.begin(), _cards.begin() + i, _cards.begin() + i + 3);

            for (int j = 3; j < 6; j++)
            {
                if (is_same_n(tmp, j, 2))
                {
                    std::vector<single_card> ret_tmp = tmp;
                    ret_tmp.erase(ret_tmp.begin() + j, ret_tmp.begin() + j + 2);
                    ret_tmp.insert(ret_tmp.begin() + 3, tmp.begin() + j, tmp.begin() + j + 2);
                    return card_pattern(card_pattern::flush, ret_tmp.data());
                }
            }

            return card_pattern(card_pattern::set, tmp.data());
        }
    }

    return card_pattern();
}

card_pattern make_pairs(const wait_pat &_cards)
{
    for (int i = 0; i < 6; i++)
    {
        if (is_same_n(_cards, i, 2))
        {
            std::vector<single_card> tmp = _cards;
            tmp.erase(tmp.begin() + i, tmp.begin() + i + 2);
            tmp.insert(tmp.begin(), _cards.begin() + i, _cards.begin() + i + 2);

            for (int j = 2; j < 6; j++)
            {
                if (is_same_n(tmp, j, 2))
                {
                    std::vector<single_card> ret_tmp = tmp;
                    ret_tmp.erase(ret_tmp.begin() + j, ret_tmp.begin() + j + 2);
                    ret_tmp.insert(ret_tmp.begin() + 2, tmp.begin() + j, tmp.begin() + j + 2);
                    return card_pattern(card_pattern::double_pair, ret_tmp.data());
                }
            }

            return card_pattern(card_pattern::pair, tmp.data());
        }
    }

    return card_pattern(card_pattern::high, _cards.data());
}

bool isStraight(single_card *astCards)
{
    bool bRet = true;

    single_card smalls[5] = {
        single_card(single_card::heart, 1),
        single_card(single_card::heart, 5),
        single_card(single_card::heart, 4),
        single_card(single_card::heart, 3),
        single_card(single_card::heart, 2),
    };
    single_card bigs[5] = {
        single_card(single_card::heart, 1),
        single_card(single_card::heart, 13),
        single_card(single_card::heart, 12),
        single_card(single_card::heart, 11),
        single_card(single_card::heart, 10),
    };

    if (astCards[0].m_num == 1)
    {
        bool isbigs = true;
        for (int i = 0; i < 5; i++)
        {
            if (astCards[i] != bigs[i])
            {
                isbigs = false;
                break;
            }
        }

        if (isbigs)
        {
            return true;
        }

        bool isSmall = true;
        for (int i = 0; i < 5; i++)
        {
            if (astCards[i] != smalls[i])
            {
                isSmall = false;
                break;
            }
        }

        if (isSmall)
        {
            for (int i = 0; i < 4; i++)
            {
                auto tmp = astCards[i];
                astCards[i] = astCards[i+1];
                astCards[i+1] = tmp;
            }
            return true;
        }

        return false;
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (1 != (astCards[i].m_num - astCards[i + 1].m_num))
            {
                bRet = false;
                break;
            }
        }
    }

    return bRet;
}

bool isFlush(single_card *astCards)
{
    bool bRet = true;

    auto col = astCards[0].m_color;
    for (int i = 1; i < 5; i++)
    {
        if (astCards[i].m_color != col)
        {
            bRet = false;
            break;
        }
    }

    return bRet;
}

bool hasSame(single_card *astCards, int _first_pos, int _n)
{
    bool bRet = true;

    if (_first_pos > 5 - _n)
    {
        return false;
    }

    auto first = astCards[_first_pos];
    for (int i = _first_pos + 1; i < _n + _first_pos; i++)
    {
        if (first != astCards[i])
        {
            bRet = false;
            break;
        }
    }
    if (false == bRet)
    {
        return hasSame(astCards, _first_pos + 1, _n);
    }
    else
    {
        int swap_pos = _first_pos - 1;
        while (swap_pos >= 0)
        {
            for (int i = 0; i < _n; i++)
            {
                auto tmp = astCards[swap_pos + i];
                astCards[swap_pos + i] = astCards[swap_pos + i + 1];
                astCards[swap_pos + i + 1] = tmp;
            }
            swap_pos--;
        }
    }

    return bRet;
}

bool isFullHouse(single_card *astCards)
{
    bool bRet = false;

    if (hasSame(astCards, 0, 3))
    {
        if (astCards[3] == astCards[4])
        {
            bRet = true;
        }
    }

    return bRet;
}

bool isTwoPair(single_card *astCards)
{
    bool bRet = false;

    if (hasSame(astCards, 0, 2))
    {
        if (hasSame(astCards, 2, 2))
        {
            bRet = true;
            if (astCards[0] < astCards[2])
            {
                auto tmp = astCards[0];
                astCards[0] = astCards[2];
                astCards[2] = tmp;
                tmp = astCards[1];
                astCards[1] = astCards[3];
                astCards[3] = tmp;
            }
        }
    }

    return bRet;
}

card_pattern card_pattern::from5cards(single_card *astCards)
{
    for (int j = 0; j < 4; j++)
    {
        bool bswap = false;
        for (int i = 0; i < 4 - j; i++)
        {
            if (astCards[i] < astCards[i + 1])
            {
                single_card tmp = astCards[i];
                astCards[i] = astCards[i + 1];
                astCards[i + 1] = tmp;
                bswap = true;
            }
        }
        if (false == bswap)
        {
            break;
        }
    }

    if (isStraight(astCards) && isFlush(astCards))
    {
        return card_pattern(card_pattern::straight_flush, astCards);
    }
    if (hasSame(astCards, 0, 4))
    {
        return card_pattern(card_pattern::fok, astCards);
    }
    if (isFullHouse(astCards))
    {
        return card_pattern(card_pattern::fullhouse, astCards);
    }
    if (isFlush(astCards))
    {
        return card_pattern(card_pattern::flush, astCards);
    }
    if (isStraight(astCards))
    {
        return card_pattern(card_pattern::straight, astCards);
    }
    if (hasSame(astCards, 0, 3))
    {
        return card_pattern(card_pattern::set, astCards);
    }
    if (isTwoPair(astCards))
    {
        return card_pattern(card_pattern::double_pair, astCards);
    }
    if (hasSame(astCards, 0, 2))
    {
        return card_pattern(card_pattern::pair, astCards);
    }

    return card_pattern(card_pattern::high, astCards);
}
card_pattern card_pattern::MakeFinalPattern(single_card *astCards)
{
    wait_pat tmp;
    for (int i = 0; i < 7; i++)
    {
        tmp.push_back(astCards[i]);
    }

    std::sort(tmp.begin(), tmp.end());
    std::reverse(tmp.begin(), tmp.end());

    std::vector<card_pattern> prob_patterns;
    prob_patterns.push_back(make_straight(tmp));
    prob_patterns.push_back(make_flush(tmp));
    prob_patterns.push_back(make_fok(tmp));
    prob_patterns.push_back(make_fullhouse_or_set(tmp));
    prob_patterns.push_back(make_pairs(tmp));



    // for (int j = 0; j < 6; j++)
    // {
    //     bool bswap = false;
    //     for (int i = 0; i < 6 - j; i++)
    //     {
    //         if (astCards[i] < astCards[i + 1])
    //         {
    //             single_card tmp = astCards[i];
    //             astCards[i] = astCards[i + 1];
    //             astCards[i + 1] = tmp;
    //             bswap = true;
    //         }
    //     }
    //     if (false == bswap)
    //     {
    //         break;
    //     }
    // }
    // single_card lasttime[5];
    // memcpy(lasttime, astCards, sizeof(lasttime));
    // prob_patterns.push_back(card_pattern::from5cards(astCards));

    // if (memcmp(lasttime, astCards, sizeof(lasttime)) || astCards[0] == astCards[1])
    // {
    //     prob_patterns.push_back(card_pattern::from5cards(astCards + 2));
    //     prob_patterns.push_back(card_pattern::from5cards(astCards));
    // }
    // prob_patterns.push_back(card_pattern::from5cards(astCards + 1));
    // prob_patterns.push_back(card_pattern::from5cards(astCards + 2));
    // prob_patterns.push_back(card_pattern::from5cards(astCards));

    // std::vector<single_card> col_cards[single_card::invalid];
    // for (size_t i = 0; i < 7; i++)
    // {
    //     col_cards[astCards[i].m_color].push_back(astCards[i]);
    // }

    // for (size_t i = 0; i < single_card::invalid; i++)
    // {
    //     if (col_cards[i].size() >= 5)
    //     {
    //         std::sort(col_cards[i].begin(), col_cards[i].end());
    //         col_cards[i].reserve(col_cards[i].size());
    //         prob_patterns.push_back(card_pattern::from5cards(col_cards[i].data()));
    //         if (col_cards[i].size() == 6)
    //         {
    //             prob_patterns.push_back(card_pattern::from5cards(col_cards[i].data() + 1));
    //         }
    //         if (col_cards[i].size() == 7)
    //         {
    //             prob_patterns.push_back(card_pattern::from5cards(col_cards[i].data() + 2));
    //         }
    //         break;
    //     }
    // }

    std::sort(prob_patterns.begin(), prob_patterns.end());

    return prob_patterns[prob_patterns.size() - 1];
}

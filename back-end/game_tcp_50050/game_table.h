#if !defined(_GAME_TABLE_H)
#define _GAME_TABLE_H

#include <string>
#include <vector>
#include <random>
#include <iostream>
#include "timer_channel.h"

class single_card
{
public:
    enum color_em
    {
        heart,
        spade,
        diamond,
        club,
        invalid
    };
    color_em m_color = invalid;
    int m_num = 0;
    single_card() {}
    single_card(color_em _color, int _num) : m_color(_color), m_num(_num) {}

    bool operator>(const single_card &_orig)
    {
        if (m_num == 1 && _orig.m_num != 1)
        {
            return true;
        }
        else if (m_num == 1 && _orig.m_num == 1)
        {
            return false;
        }
        else if (m_num != 1 && _orig.m_num == 1)
        {
            return false;
        }

        return this->m_num > _orig.m_num;
    }
    bool operator>=(const single_card &_orig)
    {
        if (m_num == 1 && _orig.m_num != 1)
        {
            return true;
        }
        else if (m_num == 1 && _orig.m_num == 1)
        {
            return true;
        }
        else if (m_num != 1 && _orig.m_num == 1)
        {
            return false;
        }
        return this->m_num >= _orig.m_num;
    }
    bool operator<(const single_card &_orig)
    {
        if (m_num == 1 && _orig.m_num != 1)
        {
            return false;
        }
        else if (m_num == 1 && _orig.m_num == 1)
        {
            return false;
        }
        else if (m_num != 1 && _orig.m_num == 1)
        {
            return true;
        }
        return this->m_num < _orig.m_num;
    }
    bool operator<=(const single_card &_orig)
    {
        if (m_num == 1 && _orig.m_num != 1)
        {
            return false;
        }
        else if (m_num == 1 && _orig.m_num == 1)
        {
            return true;
        }
        else if (m_num != 1 && _orig.m_num == 1)
        {
            return true;
        }
        return this->m_num <= _orig.m_num;
    }

    bool operator==(const single_card &_orig)
    {
        return this->m_num == _orig.m_num;
    }
    bool operator!=(const single_card &_orig) const
    {
        return this->m_num != _orig.m_num;
    }
    int operator-( const single_card &_orig) const
    {
        int first = m_num == 1? 14: m_num;
        int second = _orig.m_num;

        return first - second;
    }
};

class cards_heap
{
    std::vector<single_card> m_cards;

public:
    cards_heap();
    single_card PopOneCard();
};
class action_in_table
{
public:
    enum action_type
    {
        call,
        bat,
        raise,
        fall,
        check,
        allin,
        invalid
    } action;
    int cash;
    int action_no;
};

class card_pattern
{
public:
    enum patten_type
    {
        high,
        pair,
        double_pair,
        set,
        straight,
        flush,
        fullhouse,
        fok,
        straight_flush
    };

private:
    patten_type m_enType;
    single_card m_final_pattern[5];

public:
    card_pattern(patten_type _type, const single_card *astCards) : m_enType(_type)
    {
        for (int i = 0; i < 5; i++)
        {
            m_final_pattern[i] = astCards[i];
        }
    }
    card_pattern() : m_enType(high) {}
    patten_type GetType()
    {
        return m_enType;
    }
    single_card GetFinalCard(int _n)
    {
        return m_final_pattern[_n];
    }

    bool operator<(const card_pattern &_orig)
    {
        bool bRet = false;

        if (m_enType < _orig.m_enType)
        {
            bRet = true;
        }
        else if (m_enType == _orig.m_enType)
        {
            for (int i = 0; i < 5; i++)
            {
                if (m_final_pattern[i] < _orig.m_final_pattern[i])
                {
                    bRet = true;
                    break;
                }
                else if (m_final_pattern[i] > _orig.m_final_pattern[i])
                {
                    break;
                }
            }
        }

        return bRet;
    }
    bool operator==(const card_pattern &_orig)
    {
        bool bRet = false;

        if (m_enType == _orig.m_enType)
        {
            bRet = true;
            for (int i = 0; i < 5; i++)
            {
                if (m_final_pattern[i] != _orig.m_final_pattern[i])
                {
                    bRet = false;
                    break;
                }
            }
        }

        return bRet;
    }
    bool operator>(const card_pattern &_orig)
    {
        bool bRet = true;
        if (*this < _orig || *this == _orig)
        {
            bRet = false;
        }

        return bRet;
    }
    static card_pattern from5cards(single_card *astCards);
    static card_pattern MakeFinalPattern(single_card *astCards);
};

class game_table;
class game_player:public timeout_task
{
    std::string m_szName;
    int m_iCash;
    std::string m_ssid;
    int m_iBatCash = 0;
    int m_TableNo = -1;
    single_card m_hand_cards[2];
    bool m_isReady = false;
    bool m_falled = false;
    bool m_Allined = false;
    card_pattern m_pattern;

public:
    game_player(std::string _name, std::string _ssid, int _cash);
    int GetBatCash()
    {
        return m_iBatCash;
    }
    void SetBatCash(int _iCash)
    {
        m_iBatCash = _iCash;
    }
    int GetCurCash()
    {
        return m_iCash;
    }
    std::string GetName()
    {
        return m_szName;
    }
    int m_iSeatPos;
    std::string GetSSID()
    {
        return m_ssid;
    }

    void SetTable(int _no)
    {
        m_TableNo = _no;
    }
    int GetTable()
    {
        return m_TableNo;
    }
    single_card *GetHandCard(int _i)
    {
        return &m_hand_cards[_i];
    }
    void SetReady(bool _ready)
    {
        m_isReady = _ready;
    }
    bool IsReady()
    {
        return m_isReady;
    }
    void SetHandCards(single_card *_2cards)
    {
        m_hand_cards[0] = _2cards[0];
        m_hand_cards[1] = _2cards[1];
        SyncOut();
    }
    int BatCashOut(int _cash, game_table *_pTable);
    virtual void SyncOut();
    void SetFalled(bool _fall)
    {
        m_falled = _fall;
    }
    bool IsFalled()
    {
        return m_falled;
    }
    int ExecAction(const action_in_table &_action, game_table *_pgame_table);
    void SetAllin(bool _allin)
    {
        m_Allined = _allin;
    }
    bool IsAllin()
    {
        return m_Allined;
    }
    void ResetDefault()
    {
        m_hand_cards[0] = single_card();
        m_hand_cards[1] = single_card();
        m_Allined = false;
        m_falled = false;
        m_iBatCash = 0;
        SyncOut();
    }
    void SetPattern(const card_pattern &_pattern)
    {
        m_pattern = _pattern;
    }
    card_pattern GetPattern()
    {
        return m_pattern;
    }
    void IncreatseCash(int _cash)
    {
        m_iCash += _cash;
    }
	virtual void proc_timeout();
};

class game_table;
class AbsState
{
public:
    virtual AbsState *work(game_table *_pgame_table) { return this; }
    virtual AbsState *procStart(game_table *_pgame_table) { return this; }
    virtual AbsState *procStop(game_table *_pgame_table) { return this; }
    virtual AbsState *procAction(game_table *_pgame_table, action_in_table &_action) { return this; }
    bool bat_or_continue(game_table *_pgame_table, action_in_table &_action);
    virtual std::string GetCurrentStateName() {return "";}
    void log_current_state_out() {
        std::cout << "+++++enter " << GetCurrentStateName() << "+++++++" <<std::endl;
    }
    virtual ~AbsState() {}
};

class game_state_init : public AbsState
{
    virtual AbsState *procStart(game_table *_pgame_table);
    virtual std::string GetCurrentStateName() {
        return "init state";
    }
public:
    game_state_init() {
        log_current_state_out();
    }
};

class game_state_preFlop : public AbsState
{
    virtual AbsState *work(game_table *_pgame_table);
    virtual AbsState *procAction(game_table *_pgame_table, action_in_table &_action);
    virtual std::string GetCurrentStateName() {
        return "preflop state";
    }
public:
    game_state_preFlop() {
        log_current_state_out();
    }
};

class game_state_Flop : public AbsState
{
    virtual AbsState *work(game_table *_pgame_table);
    virtual AbsState *procAction(game_table *_pgame_table, action_in_table &_action);
    virtual std::string GetCurrentStateName() {
        return "flop state";
    }
public:
    game_state_Flop() {
        log_current_state_out();
    }
};
class game_state_Turn : public AbsState
{
    virtual AbsState *work(game_table *_pgame_table);
    virtual AbsState *procAction(game_table *_pgame_table, action_in_table &_action);
    virtual std::string GetCurrentStateName() {
        return "turn state";
    }
public:
    game_state_Turn() {
        log_current_state_out();
    }
};
class game_state_River : public AbsState
{
    virtual AbsState *work(game_table *_pgame_table);
    virtual AbsState *procAction(game_table *_pgame_table, action_in_table &_action);
    virtual std::string GetCurrentStateName() {
        return "river state";
    }
public:
    game_state_River() {
        log_current_state_out();
    }
};
class game_state_Finish : public AbsState
{
    virtual AbsState *work(game_table *_pgame_table);
    virtual std::string GetCurrentStateName() {
        return "finish state";
    }
public:
    game_state_Finish() {
        log_current_state_out();
    }
};

class game_table: public timeout_task
{
    AbsState *m_pCurState = new game_state_init();
    game_player *m_apxPlayer[6] = {0};
    int m_main_pool = 0;
    int m_side_pool[6] = {0};
    int m_curDealer_pos = 0;
    single_card m_table_cards[5];
    bool m_bAllReady = false;
    cards_heap m_heap;
    int m_small_blind = 10;
    int m_iActionPos = -1;
    int m_minBatCash = 0;
    int m_LastActionSeat = 0;

public:
    game_table(int _no) : m_table_no(_no) {}
    int m_table_no;
    bool AddPlayer(int _seat, game_player *_player);
    bool AddPlayer(game_player *_player);
    void DelPlayer(int _seat);
    game_player *GetPlayerBySeat(int _seat);
    void Start();
    void Stop();
    void ProcAction(action_in_table &_action);
    int GetNumberOnTable();
    int GetCurBatCash()
    {
        return m_main_pool;
    }
    int GetCurDealerPos()
    {
        return m_curDealer_pos;
    }
    single_card *GetTableCard(int _index)
    {
        return &m_table_cards[_index];
    }
    bool IsStarted()
    {
        return m_bAllReady;
    }
    void SetStartState(bool _start)
    {
        m_bAllReady = _start;
    }
    single_card CardPop()
    {
        return m_heap.PopOneCard();
    }
    void ResetHeap()
    {
        m_heap = cards_heap();
    }
    void InitDealerPos();
    void MoveDealer();
    game_player *GetNextPlayer(int _seat);
    int GetSmallBlind()
    {
        return m_small_blind;
    }
    void MainPoolinCash(int _cash)
    {
        m_main_pool += _cash;
    }
    void SetActionPos(int _seat)
    {
        m_iActionPos = _seat;
        if (GetPlayerBySeat(m_iActionPos)->IsAllin() || GetPlayerBySeat(m_iActionPos)->IsFalled())
        {
            SetActionPos(GetNextPlayer(m_iActionPos)->m_iSeatPos);
        }
        else
        {
            timeout_deliver::GetInstance()->register_task(20, GetPlayerBySeat(_seat));
        }

    }
    int GetActionPos()
    {
        return m_iActionPos;
    }
    void SetMinBatCash(int _cash)
    {
        m_minBatCash = _cash;
    }
    int GetMinBatCash()
    {
        return m_minBatCash;
    }
    void SetLastActionSeat(int _seat)
    {
        auto player = GetPlayerBySeat(_seat);
        if (player->IsAllin() || player->IsFalled())
        {
            SetLastActionSeat(GetPrevPlayer(_seat)->m_iSeatPos);
        }
        else
        {
            m_LastActionSeat = _seat;
        }
    }
    int GetLastActionSeat()
    {
        return m_LastActionSeat;
    }
    void ResetActionPos() {
        m_iActionPos = -1;
    }
    void SetTableCard(int _i, const single_card &_card)
    {
        m_table_cards[_i] = _card;
    }
    game_player *GetPrevPlayer(int _seat);
    void DeliverPool();
    int GetValidNum();
    int GetSidePool(int _seat)
    {
        return m_side_pool[_seat];
    }
    void DecreasePool(int _cash);
    virtual void showout_result();
	virtual void proc_timeout();
    void ResetAllPlayer() {
        for (auto itr:m_apxPlayer)
        {
            if (nullptr !=itr)
            {
                itr->SetAllin(false);
                itr->SetFalled(false);
            }
        }
    }
};

#endif // _GAME_TABLE_H

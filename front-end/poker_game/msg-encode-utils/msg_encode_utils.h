#if !defined(_MSG_ENCODE_UTILS_H_)
#define _MSG_ENCODE_UTILS_H_

#include <string.h>
#include <iostream>
#include <cstring>
#define GAME_MSG_SSID_LEN (32)
#define GAME_MSG_PLAYER_NAME_MAXLEN (32)

#ifdef QT_MSG_UTILS
#include <QtEndian>
inline int ntohl(int _orig){
    return qFromBigEndian(_orig);
}
#else
#include <arpa/inet.h>
#endif


enum game_msg_type_em
{
    game_msg_type_invalid,
    game_msg_type_enter_table,
    game_msg_type_table_info,
    game_msg_type_handcard_info,
    game_msg_type_create_table,
    game_msg_type_heart,
    game_msg_type_ready,
    game_msg_type_action,
    game_msg_type_last_result,
    game_msg_type_max
};

enum game_msg_game_state {
    game_msg_game_state_ready,
    game_msg_game_state_start,
    game_msg_game_state_max
};

struct game_msg_heart {
    char content[6];
    void convert() {}
};

struct game_msg_ready {
    int bReady;
    void convert(){
        bReady = ntohl(bReady);
    }
};

enum game_msg_card_color
{
    game_card_heart, game_card_spade, game_card_diamond, game_card_club, game_card_invalid
};

struct game_msg_card
{
    game_msg_card_color color = game_card_invalid;
    int number = 0;
    void convert(){
        number = ntohl(number);
        color = (game_msg_card_color)ntohl(color);
    }
    game_msg_card() {}
    game_msg_card(bool _fake) {
        (void)(_fake);
        number = 4;
        color = game_card_diamond;
    }
};

struct game_msg_enter_table {
    int iTableId = -1;
    void convert() {
        iTableId = ntohl(iTableId);
    }
    game_msg_enter_table(){}
    game_msg_enter_table(bool _fake) {
        (void)(_fake);
        iTableId = 2;
    }
};

struct game_msg_player_info {
    char szName[GAME_MSG_PLAYER_NAME_MAXLEN] = "";
    int icurrentCash = 0;
    int ibatCash = 0;
    int iSeatPos = -1;
    int bReady = 0;
    int bFalled = 1;
    int bAllined = 0;
    void convert() {
        icurrentCash = ntohl(icurrentCash);
        ibatCash = ntohl(ibatCash);
        iSeatPos = ntohl(iSeatPos);
        bReady = ntohl(bReady);
        bFalled = ntohl(bFalled);
        bAllined = ntohl(bAllined);
    }
    game_msg_player_info() {}
    game_msg_player_info(int _iSeatPos):szName("Mark") {
        icurrentCash = 60;
        ibatCash = 20;
        iSeatPos = _iSeatPos;
    }
};

struct game_msg_table_info {
    int iTableNo = -1;
    int iCurPos = -1;
    int iCurBatCash = -1;
    int iActionPos = -1;
    int iMinBatCash = 0;
    game_msg_card astTableCards[5];
    game_msg_player_info astAllPlayer[6];
    game_msg_game_state enState = game_msg_game_state_max;
    void convert() {
        iTableNo = ntohl(iTableNo);
        iCurBatCash = ntohl(iCurBatCash);
        iCurPos = ntohl(iCurPos);
        enState = (game_msg_game_state)ntohl(enState);
        iActionPos = ntohl(iActionPos);
        iMinBatCash = ntohl(iMinBatCash);
        for (auto &itr:astTableCards)
            itr.convert();
        for (auto &itr:astAllPlayer)
            itr.convert();
    }
    game_msg_table_info() {}
    game_msg_table_info(bool _fake) {
        (void)(_fake);
        iTableNo = 2;
        iCurPos = 3;
        iCurBatCash = 560;
        for (size_t i = 0; i < 5; i++)
        {
            game_msg_card fake_card(true);
            memcpy(&astTableCards[i], &fake_card, sizeof(fake_card));
        }
        for (size_t i = 0; i < 6; i++)
        {
            game_msg_player_info fake_player(i);
            memcpy(&astAllPlayer[i], &fake_player, sizeof(fake_player));
        }
    }
};

struct game_msg_hand_card_info {
    int iSelfSeat = -1;
    game_msg_card hand_card[2];
    game_msg_hand_card_info(){}
    void convert() {
        iSelfSeat = ntohl(iSelfSeat);
        for (auto &itr:hand_card)
            itr.convert();
    }
};

struct game_msg_create_table {
    game_msg_create_table() {}
    game_msg_create_table(bool _fake) {
        (void)(_fake);
    }
    void convert(){}
};

enum game_msg_action_em {
    game_msg_action_call,
    game_msg_action_bat,
    game_msg_action_raise,
    game_msg_action_check,
    game_msg_action_fall,
    game_msg_action_allin,
    game_msg_action_max
};

struct game_msg_action {
    game_msg_action_em enAction = game_msg_action_max;
    int iCash = 0;
    void convert() {
        enAction = (game_msg_action_em)ntohl(enAction);
        iCash = ntohl(iCash);
    }
};

enum game_msg_pattern_type {
    game_msg_pattern_type_hight,
    game_msg_pattern_type_pair,
    game_msg_pattern_type_two_pair,
    game_msg_pattern_type_set,
    game_msg_pattern_type_straiht,
    game_msg_pattern_type_flush,
    game_msg_pattern_type_fullhouse,
    game_msg_pattern_type_fok,
    game_msg_pattern_type_flush_straight,
    game_msg_pattern_type_max
};

struct game_msg_single_result {
    int bShow = 0;
    game_msg_pattern_type enType = game_msg_pattern_type_max;
    game_msg_card finalPattern[5];
    void convert() {
        bShow = ntohl(bShow);
        enType = (game_msg_pattern_type)ntohl(enType);
        for (auto &itr:finalPattern) {
            itr.convert();
        }
    }
};

struct game_msg_last_result {
    game_msg_single_result allresult[6];
    void convert() {
        for (auto &itr:allresult) {
            itr.convert();
        }
    }
};

int msg_GetBuffLen(game_msg_type_em _enType);
game_msg_type_em msg_GetType(const char *_pcBuff, int _iLen);
int MSG_GetMsgLenFrombuff(const char *pcBuff, int _iLen);
bool MSG_GetSSIDFromBuff(const char *pcBuff, int _iLen, char *_pcSsid);

#define MSG_ENCODE_MIN(x, y) ((x) < (y) ? (x) : (y))
#define MSG_GET_TOTAL_LEN_BY_TYPE(x) (sizeof(x) + sizeof(game_msg_type_em) + sizeof(int) + GAME_MSG_SSID_LEN)
#define MSG_MIN_LEN() (sizeof(int) + sizeof(game_msg_type_em) + GAME_MSG_SSID_LEN)
#define COPY_MSG_FROM_BUFF(buff,len,type,out,ssid) do {\
    int iOrigLen = MSG_GetMsgLenFrombuff(buff, len);\
    int iCurLen = sizeof(type);\
    if (iOrigLen >= 0)\
{\
    int iCopyLen = MSG_ENCODE_MIN(iOrigLen, iCurLen);\
    memcpy(out, buff + GAME_MSG_SSID_LEN + sizeof(game_msg_type_em) + sizeof(int), iCopyLen);\
    memcpy(ssid, buff, GAME_MSG_SSID_LEN);\
    }\
    } while(0)
#define APPEND_MSG_TO_BUFF(ssid,msg,out,type_en,type,tmp) do {\
    memcpy(out, ssid, GAME_MSG_SSID_LEN);\
    game_msg_type_em enType = type_en;\
    enType = (game_msg_type_em)ntohl(enType);\
    memcpy(out+ GAME_MSG_SSID_LEN, &enType, sizeof(enType));\
    int iLen = sizeof(type);\
    iLen = ntohl(iLen);\
    memcpy(out+ GAME_MSG_SSID_LEN + sizeof(game_msg_type_em), &iLen, sizeof(iLen));\
    memcpy(out+ GAME_MSG_SSID_LEN + sizeof(game_msg_type_em) + sizeof(int), msg, sizeof(type));\
    } while(0);\
    type *tmp= (type *)(out+ GAME_MSG_SSID_LEN +sizeof(game_msg_type_em) + sizeof(int))


#define MSG_DECODE(buff, len, out, ssid) do {\
    typeof(out) *pout = &out;\
    COPY_MSG_FROM_BUFF(buff,len,typeof(out),pout,ssid);\
    pout->convert();\
    } while(0)

#define MSG_ENCODE(msg,ssid,type_en,buff) do {\
    typeof(msg) *pmsg = &msg;\
    APPEND_MSG_TO_BUFF(ssid, pmsg, buff, type_en, typeof(msg),tmp);\
    tmp->convert();\
    } while(0)

class Imsg_processor {
public:
    std::string tmp;
    virtual void proc_msg(game_msg_enter_table &_msg, char *ssid) {
        (void)(_msg);
        (void)(ssid);
    }
    virtual void proc_msg(game_msg_table_info &_msg, char *ssid) {(void)(_msg);
                                                                  (void)(ssid);}
    virtual void proc_msg(game_msg_hand_card_info &_msg, char *ssid) {(void)(_msg);
                                                                      (void)(ssid);}
    virtual void proc_msg(game_msg_create_table &_msg, char *ssid) {(void)(_msg);
                                                                    (void)(ssid);}
    virtual void proc_msg(game_msg_ready &_msg, char *ssid) {(void)(_msg);
                                                             (void)(ssid);}
    virtual void proc_msg(game_msg_action &_msg, char *ssid) {(void)(_msg);
                                                              (void)(ssid);}
    virtual void proc_msg(game_msg_last_result &_msg, char *ssid) {(void)(_msg);
                                                                   (void)(ssid);}
};
void recv_and_proccess_msg(const char *_pcBuff, int _iBuffLen, Imsg_processor *_processor);
#endif // _MSG_ENCODE_UTILS_H_

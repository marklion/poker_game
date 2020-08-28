#if !defined(_GAME_TCP_CHANNEL_H_)
#define _GAME_TCP_CHANNEL_H_
#include <zinx.h>
#include <ZinxTCP.h>
#include <stdio.h>
#include <string>
#include "game_msg.h"
#include "game_msg_router.h"

class game_tcp_handler:public AZinxHandler
{
    ZinxTcpData *m_bind_channel = NULL;
    std::string m_buffer;
    /*信息处理函数，开发者重写该函数实现信息的处理，当有需要一个环节继续处理时，应该创建新的信息对象（堆对象）并返回指针*/
	virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput){
        GET_REF2DATA(BytesMsg, input, _oInput);
        m_buffer.append(input.szData);
        while (m_buffer.length() >= 8)
        {
            int *pTlvHeader = (int *)m_buffer.data();
            int iType = ntohl(pTlvHeader[0]);
            int iLen = ntohl(pTlvHeader[1]);
            if (m_buffer.length() - 8 < (unsigned int)iLen) {
                break;
            }

            auto pGameMsg = new game_msg(iType, iLen, m_buffer.data() + 8, m_bind_channel);
            game_msg_router::GetRouter().ProcMsgEntry(pGameMsg);
            delete pGameMsg;
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + 8 + iLen);
        }
        return NULL;
    }

	/*获取下一个处理环节函数，开发者重写该函数，可以指定当前处理环节结束后下一个环节是什么，通过参数信息对象，可以针对不同情况进行分别处理*/
	virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg)
    {
        return NULL;
    }
public:
    void SetBindChannel(ZinxTcpData *_bind_channel) {
        m_bind_channel = _bind_channel;
    }
};

class game_tcp_channel:public ZinxTcpData
{
private:
    game_tcp_handler *m_handler = NULL;
public:
    game_tcp_channel(int _fd, game_tcp_handler *_handler):ZinxTcpData(_fd),m_handler(_handler) {}
	virtual AZinxHandler * GetInputNextStage(BytesMsg & _oInput) {
        m_handler->SetBindChannel(this);
        return m_handler;
    }
    virtual ~game_tcp_channel() {
        if (nullptr != m_handler)
        {
            delete m_handler;
        }
        game_msg_router::GetRouter().remove_ssid_channel(this);
    }
};

class game_tcp_fact:public IZinxTcpConnFact
{
public:
	virtual ZinxTcpData *CreateTcpDataChannel(int _fd) {
        return new game_tcp_channel(_fd, new game_tcp_handler());
    }
};

#endif // _GAME_TCP_CHANNEL_H_

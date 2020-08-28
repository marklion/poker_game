#include <zinx.h>
#include <ZinxTCP.h>
#include "game_msg_processor.h"
#include "../../front-end/poker_game/msg-encode-utils/msg_encode_utils.h"
#include "timer_channel.h"
#include "game_tcp_channel.h"

class game_session_channel:public ZinxTcpData, public timeout_task{
    char m_ssid[GAME_MSG_SSID_LEN] = {0};
public:
    game_session_channel(int _fd):ZinxTcpData(_fd){}
	virtual AZinxHandler * GetInputNextStage(BytesMsg & _oInput){
        return game_msg_processor::GetInst();
    }
	virtual bool ReadFd(std::string & _input) {
        bool bRet = false;
        if (true == ZinxTcpData::ReadFd(_input))
        {
            if (true == MSG_GetSSIDFromBuff(_input.data(), _input.length(), m_ssid))
            {
                game_msg_processor::GetInst()->SetChannelBySSID(m_ssid, this);
                bRet = true;
            }
        }

        return bRet;
    }
    virtual ~game_session_channel() {
        game_msg_processor::GetInst()->ClearChannelBySSID(m_ssid);
    }
	virtual void proc_timeout() {

    }
};

class game_ss_fac:public IZinxTcpConnFact {
	virtual ZinxTcpData *CreateTcpDataChannel(int _fd)
    {
        return new game_session_channel(_fd);
    }
};

int main(int argc, const char** argv) {
    ZinxKernel::ZinxKernelInit();
    auto plistChannel = new ZinxTCPListen(50050, new game_tcp_fact());
    ZinxKernel::Zinx_Add_Channel(*plistChannel);
    ZinxKernel::Zinx_Add_Channel(*(new timer_channel()));
    ZinxKernel::Zinx_Run();
    ZinxKernel::ZinxKernelFini();

    return 0;
}
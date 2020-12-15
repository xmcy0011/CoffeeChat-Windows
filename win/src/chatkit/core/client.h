/** @file TcpClient.h
  * @brief TcpClient
  * @author fei.xu
  * @date 2020/8/26
  */

#ifndef _TCPCLIENT_2E029671_CF33_4CE8_9F84_FF10E3867D55_
#define _TCPCLIENT_2E029671_CF33_4CE8_9F84_FF10E3867D55_

#include <string>
#include <cstdint>
#include <memory>
#include <functional>

#include "cim.h"
#include "cim_def.h"
#include "pb/CIM.Def.pb.h"
#include "pb/cim.Login.pb.h"
#include "base/noncopyable.h"

#include <google/protobuf/message_lite.h>

using namespace std;

namespace evpp {
    class TCPClient;
    class EventLoopThread;
    class Buffer;
    class TCPConn;
    typedef std::shared_ptr<evpp::TCPConn> TCPConnPtr;
}

namespace cim {
    namespace core {
        enum ConnectStatus {
            kDefault,
            kConnectting,
            kConnectOk,
            kDisConnect
        };
        typedef std::function<void(const ConnectStatus& status)> ConnectionCallback;
        typedef std::function<void(IMHeader&, evpp::Buffer* data)> MessageCallback;
        typedef std::function<void(const CIM::Login::CIMAuthTokenRsp& rsp)> LoginCallback;
        typedef std::function<void()> TimeoutCallback;

        class CIM_DLL_API Client : public cim::noncopyable {
          public:
            /** @fn
              * @brief ��ȡ�������̰߳�ȫ
              * @param
              * @return
              */
            static Client* getInstance();

            /** @fn
              * @brief ���ӣ����Զ����������̰߳�ȫ
              * @param
              * @return
              */
            void login(std::string user_name, std::string pwd, string ip, uint16_t port, const LoginCallback& cb, const TimeoutCallback& timeout_cb);
            void setConnectionCallback(const ConnectionCallback& cb);
            void logout();

            ConnectStatus connStatus();

            int sendRaw(const char* data, const int& len);

            int send(CIM::Def::CIMCmdID cmd_id, const google::protobuf::MessageLite& msg);

            int sendRequest();

          private:
            void onConnectionStatusChanged(const evpp::TCPConnPtr& conn);
            void onMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* buffer);
            void onHandleData(const IMHeader* header, evpp::Buffer* buffer);
            uint16_t getSeq();

            void onHandleAuthRsp(const IMHeader* header, evpp::Buffer* buffer);

          protected:
            Client();
            ~Client();

          private:
            std::unique_ptr<evpp::TCPClient> tcp_client_;
            std::unique_ptr<evpp::EventLoopThread> loop_;

            ConnectStatus conn_status_;
            LoginCallback login_cb_;
            TimeoutCallback login_timeout_cb_;
            atomic<bool> is_login_;

            uint64_t user_id_;
            std::string user_token_;

            ConnectionCallback connection_cb_;
            atomic<uint16_t> seq_;
        };
    }
}

#endif//_TCPCLIENT_2E029671_CF33_4CE8_9F84_FF10E3867D55_
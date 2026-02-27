#ifndef SRC_OPC_UA_SERVER_CHANNEL_H
#define SRC_OPC_UA_SERVER_CHANNEL_H

#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "PySysLinkBase/FullySupportedSignalValue.h"

#include "ICommunicationChannel.h"


namespace BlockTypeSupports::BasicCommunicationSupport
{
    class OPCUAServerChannel : public ICommunicationChannel
    {
        public:
            OPCUAServerChannel(std::vector<VariableDefinition> variableDefinitions, uint16_t port);
            ~OPCUAServerChannel();

            PySysLinkBase::FullySupportedSignalValue GetValue(std::string VariableName) override;
            void SetValue(std::string VariableName, PySysLinkBase::FullySupportedSignalValue value) override;
    
        private:
            UA_Server* server;
            std::thread serverThread;
            std::mutex serverMutex;
            std::mutex startupMutex;
            std::condition_variable startupCv;
            bool serverStarted = false;

            std::unordered_map<std::string, UA_NodeId> variableNodes;
            
            volatile UA_Boolean running{true};

            UA_StatusCode startupStatus = UA_STATUSCODE_GOOD;
            std::string startupErrorMessage;

            void runServer();

            static UA_Variant toVariant(const PySysLinkBase::FullySupportedSignalValue& v);
            static PySysLinkBase::FullySupportedSignalValue fromVariant(const UA_Variant& v);
    };
}

#endif // SRC_OPC_UA_SERVER_CHANNEL_H
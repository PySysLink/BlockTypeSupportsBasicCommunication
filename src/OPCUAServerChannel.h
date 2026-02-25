#ifndef SRC_OPC_UA_SERVER_CHANNEL_H
#define SRC_OPC_UA_SERVER_CHANNEL_H

#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <unordered_map>
#include <thread>
#include <mutex>

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

            std::unordered_map<std::string, UA_NodeId> variableNodes;

            void runServer();

            static UA_Variant toVariant(const PySysLinkBase::FullySupportedSignalValue& v);
            static PySysLinkBase::FullySupportedSignalValue fromVariant(const UA_Variant& v);
    };
}

#endif // SRC_OPC_UA_SERVER_CHANNEL_H
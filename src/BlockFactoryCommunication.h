#ifndef SRC_COMMUNICATION_BLOCK_FACTORY_H
#define SRC_COMMUNICATION_BLOCK_FACTORY_H

#include <PySysLinkBase/IBlockFactory.h>
#include <PySysLinkBase/IBlockEventsHandler.h>
#include <PySysLinkBase/ConfigurationValue.h>

#include <memory>
#include <string>
#include <map>
#include <stdexcept>
#include "spdlog/spdlog.h"

#include "OPCUAServerSimulationBlock.h"

namespace BlockTypeSupports::BasicCommunicationSupport
{

class BlockFactoryCommunication : public PySysLinkBase::IBlockFactory
{
public:
    BlockFactoryCommunication(std::map<std::string, PySysLinkBase::ConfigurationValue> pluginConfiguration)
    {
        
    }

    std::shared_ptr<PySysLinkBase::ISimulationBlock>
    CreateBlock(std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration,
                std::shared_ptr<PySysLinkBase::IBlockEventsHandler> eventHandler) override
    {

        std::string blockClass = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("BlockClass", blockConfiguration);


        spdlog::debug("Creating BasicCommunication block with block class {}", blockClass);

        if (blockClass == "OPCUAServer")
        {
            return std::make_shared<OPCUAServerSimulationBlock>(blockConfiguration, eventHandler);
        }
        else
        {
            throw std::invalid_argument("Unsupported BlockClass: " + blockClass);
        }
    }
};

} // namespace BlockTypeSupports::BasicCommunicationSupport

#endif // SRC_COMMUNICATION_BLOCK_FACTORY_H

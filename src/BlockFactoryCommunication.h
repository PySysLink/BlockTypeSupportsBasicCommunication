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
#include "ReadValueSimulationBlock.h"
#include "WriteValueSimulationBlock.h"

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

        if (blockClass == "BasicBlocksCommunication/OPCUAServer")
        {
            return std::make_shared<OPCUAServerSimulationBlock>(blockConfiguration, eventHandler);
        }
        if (blockClass == "BasicBlocksCommunication/ReadValue")
        {
            std::string variableType = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("VariableType", blockConfiguration);
            if (variableType == "Double") {
                return std::make_shared<ReadValueSimulationBlock<double>>(blockConfiguration, eventHandler);
            }
            else if (variableType == "Int") {
                return std::make_shared<ReadValueSimulationBlock<int>>(blockConfiguration, eventHandler);
            }
            else if (variableType == "Bool") {
                return std::make_shared<ReadValueSimulationBlock<bool>>(blockConfiguration, eventHandler);
            }
            else if (variableType == "String") {
                return std::make_shared<ReadValueSimulationBlock<std::string>>(blockConfiguration, eventHandler);
            }
            else
            {
                throw std::invalid_argument("Unsupported VariableType for ReadValue block: " + variableType);
            }
        }
        if (blockClass == "BasicBlocksCommunication/WriteValue")
        {
            std::string variableType = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("VariableType", blockConfiguration);
            if (variableType == "Double") {
                return std::make_shared<WriteValueSimulationBlock<double>>(blockConfiguration, eventHandler);
            }
            else if (variableType == "Int") {
                return std::make_shared<WriteValueSimulationBlock<int>>(blockConfiguration, eventHandler);
            }
            else if (variableType == "Bool") {
                return std::make_shared<WriteValueSimulationBlock<bool>>(blockConfiguration, eventHandler);
            }
            else if (variableType == "String") {
                return std::make_shared<WriteValueSimulationBlock<std::string>>(blockConfiguration, eventHandler);
            }
            else
            {
                throw std::invalid_argument("Unsupported VariableType for WriteValue block: " + variableType);
            }
        }
        else
        {
            throw std::invalid_argument("Unsupported BlockClass: " + blockClass);
        }
    }
};

} // namespace BlockTypeSupports::BasicCommunicationSupport

#endif // SRC_COMMUNICATION_BLOCK_FACTORY_H

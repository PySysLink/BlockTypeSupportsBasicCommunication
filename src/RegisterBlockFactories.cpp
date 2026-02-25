#include <PySysLinkBase/IBlockFactory.h>
#include "BlockFactoryCommunication.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include "LoggerInstance.h"

extern "C" void RegisterBlockFactories(std::map<std::string, std::shared_ptr<PySysLinkBase::IBlockFactory>>& registry, std::map<std::string, PySysLinkBase::ConfigurationValue> pluginConfiguration) {
    std::cout << "Call to RegisterBlockFactories" << std::endl;
    registry["BasicCommunication"] = std::make_shared<BlockTypeSupports::BasicCommunicationSupport::BlockFactoryCommunication>(pluginConfiguration);
    std::cout << "End of function" << std::endl;
}

extern "C" void RegisterSpdlogLogger(std::shared_ptr<spdlog::logger> logger) {
    BlockTypeSupports::BasicCommunicationSupport::LoggerInstance::SetLogger(logger);
    BlockTypeSupports::BasicCommunicationSupport::LoggerInstance::GetLogger()->debug("Logger from plugin BlockTypeSupportsBasicCommunication!");
}

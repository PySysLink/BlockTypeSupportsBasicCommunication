#include "LoggerInstance.h"

namespace BlockTypeSupports::BasicCommunicationSupport
{
    std::shared_ptr<spdlog::logger> LoggerInstance::s_logger = nullptr;
} // namespace BlockTypeSupports::BasicCommunicationSupport

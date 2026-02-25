#ifndef SRC_I_COMMUNICATION_CHANNEL_H
#define SRC_I_COMMUNICATION_CHANNEL_H

#include "PySysLinkBase/FullySupportedSignalValue.h"
#include <optional>
		
namespace BlockTypeSupports::BasicCommunicationSupport
{

struct VariableDefinition
{
    std::string Name;
    std::string DataType;
    std::optional<PySysLinkBase::FullySupportedSignalValue> InitialValue;
};

class ICommunicationChannel
{
public:
    virtual ~ICommunicationChannel() = default;

    virtual PySysLinkBase::FullySupportedSignalValue GetValue(std::string VariableName) = 0;
    virtual void SetValue(std::string VariableName, PySysLinkBase::FullySupportedSignalValue value) = 0;
};
}

#endif // SRC_I_COMMUNICATION_CHANNEL_H
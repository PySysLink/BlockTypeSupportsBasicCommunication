#include "OPCUAServerChannel.h"

#include <stdexcept>

namespace BlockTypeSupports::BasicCommunicationSupport
{

OPCUAServerChannel::OPCUAServerChannel(
    std::vector<VariableDefinition> variableDefinitions, uint16_t port)
{
    server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);

    UA_ServerConfig_setMinimal(
        config,
        port,          // port
        nullptr        // certificate (nullptr = no security)
    );
    UA_ServerConfig_setDefault(config);

    UA_NodeId parentNode = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);

    for (const auto& var : variableDefinitions)
    {
        UA_VariableAttributes attr = UA_VariableAttributes_default;

        if (var.InitialValue.has_value())
        {
            UA_Variant value = toVariant(*var.InitialValue);
            UA_Variant_copy(&value, &attr.value);
            UA_Variant_clear(&value);
        }

        attr.displayName =
            UA_LOCALIZEDTEXT_ALLOC("en-US", var.Name.c_str());

        UA_NodeId variableNodeId;
        UA_QualifiedName name =
            UA_QUALIFIEDNAME_ALLOC(1, var.Name.c_str());

        UA_StatusCode rc = UA_Server_addVariableNode(
            server,
            UA_NODEID_NULL,
            parentNode,
            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
            name,
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
            attr,
            nullptr,
            &variableNodeId
        );

        UA_QualifiedName_clear(&name);
        UA_LocalizedText_clear(&attr.displayName);

        if (rc != UA_STATUSCODE_GOOD)
        {
            throw std::runtime_error("Failed to create OPC UA variable: " + var.Name);
        }

        variableNodes[var.Name] = variableNodeId;
    }

    serverThread = std::thread(&OPCUAServerChannel::runServer, this);
}

OPCUAServerChannel::~OPCUAServerChannel()
{
    UA_Server_run_shutdown(server);
    if (serverThread.joinable())
        serverThread.join();

    UA_Server_delete(server);
}

void OPCUAServerChannel::runServer()
{
    UA_Server_run(server, nullptr);
}

PySysLinkBase::FullySupportedSignalValue OPCUAServerChannel::GetValue(std::string VariableName)
{
    std::lock_guard<std::mutex> lock(serverMutex);

    auto it = variableNodes.find(VariableName);
    if (it == variableNodes.end())
        throw std::out_of_range("Unknown OPC UA variable: " + VariableName);

    UA_Variant value;
    UA_Variant_init(&value);

    UA_StatusCode rc =
        UA_Server_readValue(server, it->second, &value);

    if (rc != UA_STATUSCODE_GOOD)
        throw std::runtime_error("Failed to read OPC UA variable: " + VariableName);

    auto result = fromVariant(value);
    UA_Variant_clear(&value);
    return result;
}

void OPCUAServerChannel::SetValue(
    std::string VariableName,
    PySysLinkBase::FullySupportedSignalValue value)
{
    std::lock_guard<std::mutex> lock(serverMutex);

    auto it = variableNodes.find(VariableName);
    if (it == variableNodes.end())
        throw std::out_of_range("Unknown OPC UA variable: " + VariableName);

    UA_Variant variant = toVariant(value);

    UA_StatusCode rc =
        UA_Server_writeValue(server, it->second, variant);

    UA_Variant_clear(&variant);

    if (rc != UA_STATUSCODE_GOOD)
        throw std::runtime_error("Failed to write OPC UA variable: " + VariableName);
}

UA_Variant OPCUAServerChannel::toVariant(
    const PySysLinkBase::FullySupportedSignalValue& v)
{
    UA_Variant var;
    UA_Variant_init(&var);

    std::visit([&](auto&& value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, int>)
            UA_Variant_setScalarCopy(&var, &value, &UA_TYPES[UA_TYPES_INT32]);
        else if constexpr (std::is_same_v<T, double>)
            UA_Variant_setScalarCopy(&var, &value, &UA_TYPES[UA_TYPES_DOUBLE]);
        else if constexpr (std::is_same_v<T, bool>)
            UA_Variant_setScalarCopy(&var, &value, &UA_TYPES[UA_TYPES_BOOLEAN]);
        else if constexpr (std::is_same_v<T, std::string>)
        {
            UA_String s = UA_STRING_ALLOC(value.c_str());
            UA_Variant_setScalarCopy(&var, &s, &UA_TYPES[UA_TYPES_STRING]);
            UA_String_clear(&s);
        }
        else if constexpr (std::is_same_v<T, std::complex<double>>)
        {
            std::string repr =
                std::to_string(value.real()) + "+" +
                std::to_string(value.imag()) + "i";
            UA_String s = UA_STRING_ALLOC(repr.c_str());
            UA_Variant_setScalarCopy(&var, &s, &UA_TYPES[UA_TYPES_STRING]);
            UA_String_clear(&s);
        }
    }, v);

    return var;
}

PySysLinkBase::FullySupportedSignalValue
OPCUAServerChannel::fromVariant(const UA_Variant& v)
{
    if (UA_Variant_isScalar(&v))
    {
        if (v.type == &UA_TYPES[UA_TYPES_DOUBLE])
            return *static_cast<double*>(v.data);
        if (v.type == &UA_TYPES[UA_TYPES_INT32])
            return *static_cast<int*>(v.data);
        if (v.type == &UA_TYPES[UA_TYPES_BOOLEAN])
            return *static_cast<bool*>(v.data);
        if (v.type == &UA_TYPES[UA_TYPES_STRING])
        {
            UA_String* s = static_cast<UA_String*>(v.data);
            return std::string(reinterpret_cast<char*>(s->data), s->length);
        }
    }

    throw std::runtime_error("Unsupported OPC UA variant type");
}

} // namespace
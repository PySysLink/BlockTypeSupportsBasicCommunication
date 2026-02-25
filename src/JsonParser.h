#pragma once

#include <nlohmann/json.hpp>
#include "ICommunicationChannel.h"

using json = nlohmann::json;
#include <fstream>

namespace BlockTypeSupports::BasicCommunicationSupport
{
static PySysLinkBase::FullySupportedSignalValue
jsonToSignalValue(const json& j, const std::string& type)
{
    if (type == "int")
        return j.get<int>();
    if (type == "double")
        return j.get<double>();
    if (type == "bool")
        return j.get<bool>();
    if (type == "string")
        return j.get<std::string>();

    throw std::runtime_error("Unsupported variable type: " + type);
}

static std::vector<VariableDefinition>
parseVariableListFile(const std::string& filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        throw std::runtime_error("Cannot open variable list file: " + filename);

    json j;
    f >> j;

    if (!j.contains("variables") || !j["variables"].is_array())
        throw std::runtime_error("Invalid variable list format");

    std::vector<VariableDefinition> vars;

    for (const auto& v : j["variables"])
    {
        VariableDefinition def;

        def.Name = v.at("name").get<std::string>();
        def.DataType = v.at("type").get<std::string>();

        if (v.contains("initial"))
            def.InitialValue =
                jsonToSignalValue(v["initial"], def.DataType);

        vars.push_back(std::move(def));
    }

    return vars;
}
}
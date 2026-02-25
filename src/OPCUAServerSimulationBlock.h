#pragma once



#include <PySysLinkBase/ISimulationBlock.h>
#include <PySysLinkBase/IBlockEventsHandler.h>
#include <PySysLinkBase/PortsAndSignalValues/InputPort.h>
#include <PySysLinkBase/PortsAndSignalValues/OutputPort.h>
#include <PySysLinkBase/SampleTime.h>
#include <PySysLinkBase/ConfigurationValue.h>
#include <spdlog/spdlog.h>

#include "JsonParser.h"
#include "OPCUAServerChannel.h"


namespace BlockTypeSupports::BasicCommunicationSupport
{
    class OPCUAServerSimulationBlock : public PySysLinkBase::ISimulationBlock
    {
        public:
            OPCUAServerSimulationBlock(std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration,
                          std::shared_ptr<PySysLinkBase::IBlockEventsHandler> eventsHandler)
                : ISimulationBlock(blockConfiguration, eventsHandler)
            {
                std::string fileName = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("VariableListFile", blockConfiguration);
                int opcUaServerPort = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<int>("OPCUAServerPort", blockConfiguration);

                auto variableDefinitions = parseVariableListFile(fileName);
                
                this->opcUaChannel = std::make_shared<OPCUAServerChannel>(variableDefinitions, opcUaServerPort);

                std::vector<PySysLinkBase::SampleTimeType> supportedSampleTimeTypes = {};
                supportedSampleTimeTypes.push_back(PySysLinkBase::SampleTimeType::discrete);
                this->sampleTime = std::make_shared<PySysLinkBase::SampleTime>(PySysLinkBase::SampleTimeType::inherited, supportedSampleTimeTypes);

                std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValue = std::make_shared<PySysLinkBase::SignalValue<std::shared_ptr<ICommunicationChannel>>>(PySysLinkBase::SignalValue<std::shared_ptr<ICommunicationChannel>>(this->opcUaChannel));
                this->outputPorts.push_back(std::make_shared<PySysLinkBase::OutputPort>(signalValue));
            }


            const std::shared_ptr<PySysLinkBase::SampleTime> GetSampleTime() const override
            {
                return this->sampleTime;
            }

            void SetSampleTime(std::shared_ptr<PySysLinkBase::SampleTime> sampleTime)
            {
                this->sampleTime = sampleTime;
            }

            std::vector<std::shared_ptr<PySysLinkBase::InputPort>> GetInputPorts() const override
            {
                return std::vector<std::shared_ptr<PySysLinkBase::InputPort>>{};
            }

            const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> GetOutputPorts() const override
            {
                return this->outputPorts;
            }

            const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> _ComputeOutputsOfBlock(
                const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime, bool isMinorStep = false) override
            {
                return this->outputPorts;
            }

            bool _TryUpdateConfigurationValue(std::string /*keyName*/, PySysLinkBase::ConfigurationValue /*value*/) override
            {
                return false;
            }
        
        private:
            std::shared_ptr<PySysLinkBase::SampleTime> sampleTime;
            std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts;
            std::shared_ptr<ICommunicationChannel> opcUaChannel;

    };
}
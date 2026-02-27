#pragma once



#include <PySysLinkBase/ISimulationBlock.h>
#include <PySysLinkBase/IBlockEventsHandler.h>
#include <PySysLinkBase/PortsAndSignalValues/InputPort.h>
#include <PySysLinkBase/PortsAndSignalValues/OutputPort.h>
#include <PySysLinkBase/SampleTime.h>
#include <PySysLinkBase/ConfigurationValue.h>
#include <spdlog/spdlog.h>

#include "ICommunicationChannel.h"


namespace BlockTypeSupports::BasicCommunicationSupport
{
    template <typename T> 
    class ReadValueSimulationBlock : public PySysLinkBase::ISimulationBlock
    {
        public:
            ReadValueSimulationBlock(std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration,
                          std::shared_ptr<PySysLinkBase::IBlockEventsHandler> eventsHandler)
                : ISimulationBlock(blockConfiguration, eventsHandler)
            {
                this->variableName = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("VariableName", blockConfiguration);
                
                
                std::vector<PySysLinkBase::SampleTimeType> supportedSampleTimeTypes = {};
                supportedSampleTimeTypes.push_back(PySysLinkBase::SampleTimeType::discrete);
                this->sampleTime = std::make_shared<PySysLinkBase::SampleTime>(PySysLinkBase::SampleTimeType::inherited, supportedSampleTimeTypes);

                std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValueIn = std::make_shared<PySysLinkBase::SignalValue<std::shared_ptr<ICommunicationChannel>>>(PySysLinkBase::SignalValue<std::shared_ptr<ICommunicationChannel>>());
                this->inputPorts.push_back(std::make_shared<PySysLinkBase::InputPort>(true, signalValueIn));

                std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue> signalValueOut = std::make_shared<PySysLinkBase::SignalValue<T>>(PySysLinkBase::SignalValue<T>());
                this->outputPorts.push_back(std::make_shared<PySysLinkBase::OutputPort>(signalValueOut));
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
                return this->inputPorts;
            }

            const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> GetOutputPorts() const override
            {
                return this->outputPorts;
            }

            const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> _ComputeOutputsOfBlock(
                const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime, bool isMinorStep = false) override
            {   
                auto inputValue = this->inputPorts[0]->GetValue();
                auto channelSignalValue = inputValue->TryCastToTyped<std::shared_ptr<ICommunicationChannel>>();
                std::shared_ptr<ICommunicationChannel> channel = channelSignalValue->GetPayload();
                if (channel == nullptr)
                {
                    spdlog::error("Failed to cast input signal value to ICommunicationChannel. Check if the input is correctly connected to a communication channel output.");
                    return this->outputPorts;
                }

                try
                {
                    T value = std::get<T>(channel->GetValue(this->variableName));
                    std::shared_ptr<PySysLinkBase::UnknownTypeSignalValue> outputValue = this->outputPorts[0]->GetValue();
                    auto outputValueSignal = outputValue->TryCastToTyped<T>();
                    outputValueSignal->SetPayload(value);
                    this->outputPorts[0]->SetValue(std::make_shared<PySysLinkBase::SignalValue<T>>(*outputValueSignal));
                }
                catch (const std::exception& e)
                {
                    spdlog::error("Error while reading variable '{}' from communication channel: {}", variableName, e.what());
                }
                
                return this->outputPorts;
            }

            bool _TryUpdateConfigurationValue(std::string /*keyName*/, PySysLinkBase::ConfigurationValue /*value*/) override
            {
                return false;
            }
        
        private:
            std::shared_ptr<PySysLinkBase::SampleTime> sampleTime;
            std::vector<std::shared_ptr<PySysLinkBase::InputPort>> inputPorts;
            std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts;
            std::string variableName;

    };
}
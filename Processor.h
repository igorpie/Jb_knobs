#pragma once
#include <JuceHeader.h>
#include "Editor.h"

class TutorialProcessor : public juce::AudioProcessor
{
public:
    TutorialProcessor()
        : parameters(*this, nullptr, juce::Identifier("Tutorial"),
            {
              std::make_unique<juce::AudioParameterFloat>("vol" ,   "VOLUME",   0.0f , 100.0f , 50.f),
            })
    {}

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override {
        float vol = parameters.getParameter("vol")->getValue();
        vol *= vol;                                                             // logarithmic sense

        auto mainInputOutput = getBusBuffer(buffer, true, 0);
        for (auto j = 0; j < buffer.getNumSamples(); ++j) {
            *mainInputOutput.getWritePointer(0, j) = *mainInputOutput.getReadPointer(0, j) * vol;
            *mainInputOutput.getWritePointer(1, j) = *mainInputOutput.getReadPointer(1, j) * vol;
        }
    }

    juce::AudioProcessorEditor* createEditor() override { return new GenericEditor(*this, parameters); }
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "jb Tutorial"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "none"; }
    void changeProgramName(int, const juce::String&) override {}

    
    void getStateInformation(juce::MemoryBlock& destData) override
    {
        auto state = parameters.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName(parameters.state.getType()))
                parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
    
private:
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* potVol = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TutorialProcessor)
};

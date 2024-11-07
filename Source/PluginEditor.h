#pragma once

#include <JuceHeader.h>
#include "SpectrumViewer.h"
#include "PluginProcessor.h"

class PluginEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    PluginEditor(PluginProcessor& processor);
    ~PluginEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    PluginProcessor& audioProcessor;
    juce::Value repaintViewerValue;         // Valor de referencia para repintado
    juce::Value detectedFrequencyValue;     // Valor de referencia para la frecuencia detectada
    SpectrumViewer spectrumViewer;          // Objeto para visualizar el espectro

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

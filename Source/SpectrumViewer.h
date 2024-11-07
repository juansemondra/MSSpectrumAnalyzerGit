#pragma once

#include <JuceHeader.h>

class SpectrumViewer : public juce::Component, public juce::Timer, public juce::Value::Listener
{
public:
    SpectrumViewer(juce::Value& repaintViewerValue, const std::vector<float>& leftChannelFFT, const std::vector<float>& rightChannelFFT, juce::Value& detectedFrequencyValue);
    ~SpectrumViewer() override;

    void setFFTData(const std::vector<float>& leftChannelFFT, const std::vector<float>& rightChannelFFT);
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void createGradientImage(); // Crea una imagen de gradiente para el mapa de calor de amplitud MS
    juce::Colour getAmplitudeColor(float amplitude); // Mapea amplitud a color
    void timerCallback() override; // Callback de temporizador para repintar la vista
    void valueChanged(juce::Value& value) override; // Callback para cambios en detectedFrequency
    float logTransformInRange0to1(float value); // Función de ayuda para transformación logarítmica

    std::vector<float> midData;
    std::vector<float> sideData;

    juce::Value repaintViewer;
    juce::Value detectedFrequency;
    const std::vector<float>& leftChannelData;
    const std::vector<float>& rightChannelData;

    juce::Image gradientImage;
    juce::Label frequencyCaption;
    int mouseXPosition;
    bool mouseMode;

    static const int frequenciesToPlot[];
    static const int numberOfFrequenciesToPlot;
    double sampleRate;
    int heightForFrequencyCaption;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumViewer)
};

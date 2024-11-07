#include "PluginEditor.h"
#include "PluginProcessor.h"

PluginEditor::PluginEditor(PluginProcessor& processor)
    : AudioProcessorEditor(&processor), audioProcessor(processor),
      spectrumViewer(repaintViewerValue, audioProcessor.getFFTMagnitudeBufferL(), audioProcessor.getFFTMagnitudeBufferR(), detectedFrequencyValue)
{
    setSize(800, 500); // Tamaño aumentado para mayor precisión
    addAndMakeVisible(spectrumViewer);
    
    startTimerHz(30); // Refresca cada 30 ms
}

PluginEditor::~PluginEditor()
{
    stopTimer();
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f); // Aumenta el tamaño de la fuente para mayor claridad
    juce::Rectangle<int> textArea = getLocalBounds().removeFromTop(40); // Espacio adicional para el texto
    g.drawFittedText("Analizador de Espectro MS", textArea, juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    // Ajusta el gráfico para dejar espacio para el texto
    spectrumViewer.setBounds(getLocalBounds().reduced(10).withTrimmedTop(40));
}

void PluginEditor::timerCallback()
{
    // Actualiza los datos FFT en SpectrumViewer con los datos de cada canal
    spectrumViewer.setFFTData(audioProcessor.getFFTMagnitudeBufferL(), audioProcessor.getFFTMagnitudeBufferR());
    repaint();
}

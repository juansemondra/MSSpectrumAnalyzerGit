#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      fft(11), // FFT de tamaño 2^11 = 2048 puntos
      fftDataL(2048, 0.0f), fftDataR(2048, 0.0f), magnitudeBuffer(1024, 0.0f),
      window(2048, juce::dsp::WindowingFunction<float>::hann) // Inicializa la ventana de Hann
{
}

PluginProcessor::~PluginProcessor()
{
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    // Asegúrate de que el buffer tenga al menos 2048 muestras (tamaño de la FFT)
    if (buffer.getNumSamples() < 2048)
        return;

    // Copia las muestras de los canales izquierdo y derecho al buffer de FFT
    for (int i = 0; i < 2048; ++i)
    {
        fftDataL[i] = buffer.getReadPointer(0)[i];
        fftDataR[i] = buffer.getReadPointer(1)[i];
    }

    // Aplica la ventana de Hann a los datos
    window.multiplyWithWindowingTable(fftDataL.data(), fftDataL.size());
    window.multiplyWithWindowingTable(fftDataR.data(), fftDataR.size());

    // Realiza la FFT en cada canal por separado
    fft.performFrequencyOnlyForwardTransform(fftDataL.data());
    fft.performFrequencyOnlyForwardTransform(fftDataR.data());
}

// Implementación de los demás métodos sin 'override'

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

bool PluginProcessor::hasEditor() const { return true; }

const juce::String PluginProcessor::getName() const { return JucePlugin_Name; }

bool PluginProcessor::acceptsMidi() const { return false; }

bool PluginProcessor::producesMidi() const { return false; }

bool PluginProcessor::isMidiEffect() const { return false; }

double PluginProcessor::getTailLengthSeconds() const { return 0.0; }

int PluginProcessor::getNumPrograms() { return 1; }

int PluginProcessor::getCurrentProgram() { return 0; }

void PluginProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }

const juce::String PluginProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }

void PluginProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) {}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

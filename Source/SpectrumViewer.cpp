#include "SpectrumViewer.h"

const int SpectrumViewer::frequenciesToPlot[] = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000};
const int SpectrumViewer::numberOfFrequenciesToPlot = 11;

SpectrumViewer::SpectrumViewer(juce::Value& repaintViewerValue, const std::vector<float>& leftChannelFFT, const std::vector<float>& rightChannelFFT, juce::Value& detectedFrequencyValue)
    : repaintViewer(repaintViewerValue), detectedFrequency(detectedFrequencyValue),
      leftChannelData(leftChannelFFT), rightChannelData(rightChannelFFT), gradientImage(juce::Image::RGB, 100, 1, false),
      mouseXPosition(0), mouseMode(false), sampleRate(44100.0), heightForFrequencyCaption(20)
{
    setOpaque(true);

    detectedFrequency.addListener(this); // Registra el listener para los cambios en detectedFrequency
    gradientImage.clear(gradientImage.getBounds(), juce::Colours::black);
    addAndMakeVisible(frequencyCaption);

    createGradientImage();
    startTimer(30); // Inicia el temporizador con un intervalo de 30 ms
}

SpectrumViewer::~SpectrumViewer()
{
    detectedFrequency.removeListener(this); // Elimina el listener en el destructor
}

void SpectrumViewer::setFFTData(const std::vector<float>& leftChannelFFT, const std::vector<float>& rightChannelFFT)
{
    midData.resize(leftChannelFFT.size());
    sideData.resize(rightChannelFFT.size());

    // Calcula los valores MS
    for (size_t i = 0; i < leftChannelFFT.size(); ++i)
    {
        midData[i] = (leftChannelFFT[i] + rightChannelFFT[i]) * 0.5f;
        sideData[i] = (leftChannelFFT[i] - rightChannelFFT[i]) * 0.5f;
    }

    repaint(); // Fuerza el repintado para reflejar los nuevos datos
}

void SpectrumViewer::timerCallback()
{
    if (repaintViewer.getValue())
    {
        repaint();
    }
}

void SpectrumViewer::valueChanged(juce::Value& value)
{
    if (value == detectedFrequency)
    {
        // Actualiza algo en base al valor de frecuencia detectado, si es necesario
    }
}

void SpectrumViewer::createGradientImage()
{
    juce::Graphics g(gradientImage);
    juce::ColourGradient gradient(juce::Colours::purple, 0, 0, juce::Colours::red, 100, 0, false);
    gradient.addColour(0.33, juce::Colours::blue);   // Bajo amplitud
    gradient.addColour(0.66, juce::Colours::green);  // Media amplitud
    gradient.addColour(1.0, juce::Colours::yellow);  // Alta amplitud
    g.setGradientFill(gradient);
    g.fillAll();
}

void SpectrumViewer::paint(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight() - heightForFrequencyCaption;

    g.setColour(juce::Colours::black);
    g.fillRect(0, 0, w, h);

    g.setColour(juce::Colours::darkgrey);
    for (int i = 0; i < numberOfFrequenciesToPlot; ++i)
    {
        const double proportion = frequenciesToPlot[i] / (sampleRate * 0.5);
        int yPos = h - logTransformInRange0to1(proportion) * h;  // Invertimos el eje Y
        g.drawHorizontalLine(yPos, 0.0f, (float)w);
    }

    // Dibuja los datos del espectro utilizando los valores MS
    for (size_t i = 0; i < midData.size(); ++i)
    {
        // Calcular la amplitud individual para cada frecuencia y obtener el color correspondiente
        float amplitude = std::sqrt(midData[i] * midData[i] + sideData[i] * sideData[i]);
        juce::Colour color = getAmplitudeColor(amplitude); // Color basado en la amplitud de cada punto
        g.setColour(color);

        int x = (int)juce::jmap((float)i / (float)midData.size(), 0.0f, 1.0f, 0.0f, (float)w); // Eje X para frecuencia
        int y = h - (int)juce::jmap(logTransformInRange0to1(amplitude), 0.0f, 1.0f, 0.0f, (float)h); // Eje Y para MS

        g.fillRect(x, y, 2, 2);
    }
}

juce::Colour SpectrumViewer::getAmplitudeColor(float amplitude)
{
    // Convertir la amplitud a dB, asegurando que no sea -inf (ajustando a un mínimo de -100 dB)
    float dbValue = juce::Decibels::gainToDecibels(amplitude, -100.0f);

    // Verificación de rango de dbValue para asegurar que esté entre -100 y 0 dB
    if (dbValue < -100.0f)
        dbValue = -100.0f;
    else if (dbValue > 0.0f)
        dbValue = 0.0f;

    // Mapear dbValue al rango [0, 1], donde -100 dB corresponde a 0 y 0 dB corresponde a 1
    float colorScale = (dbValue + 100.0f) / 100.0f; // Mapea dbValue al rango [0, 1]
    
    // Escoger el color de gradientImage en función de colorScale
    int pixelX = juce::roundToInt(colorScale * (gradientImage.getWidth() - 1)); // Mapea el valor de amplitud al ancho del gradiente
    return gradientImage.getPixelAt(pixelX, 0);
}

float SpectrumViewer::logTransformInRange0to1(float value)
{
    if (value <= 0.0f) return 0.0f;
    if (value >= 1.0f) return 1.0f;
    return std::log10(value * 9.0f + 1.0f); // Transformación logarítmica escalada al rango [0,1]
}

void SpectrumViewer::resized()
{
    // Aquí puedes definir cualquier ajuste de tamaño necesario.
    // Si no necesitas que haga nada por ahora, puedes dejarlo vacío.
}

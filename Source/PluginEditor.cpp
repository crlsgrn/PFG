/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
void LookAndFeel1::drawRotarySlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    g.setColour(Colour(0u, 0u, 0u));
    g.fillEllipse(bounds);


    g.setColour(Colour(241u, 241u, 237u));
    g.drawEllipse(bounds, 2);


    auto center = bounds.getCentre();

    Path p;

    Rectangle<float> r;
    r.setLeft(center.getX() - 2);
    r.setRight(center.getX() + 2);
    r.setTop(bounds.getY());
    r.setBottom(center.getY());

    p.addRectangle(r);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.fillPath(p);
}
//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();

    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
        sliderBounds.getX(),
        sliderBounds.getY(),
        sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
        startAng,
        endAng,
        *this);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;

    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(2);
    return r;

}
//==============================================================================

//==============================================================================
PhaserAudioProcessorEditor::PhaserAudioProcessorEditor(PhaserAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    depthSliderAttachment1(audioProcessor.apvts, "Band1.Depth", depthSlider1),
    depthSlider1(*audioProcessor.apvts.getParameter("Band1.Depth"), ""),
    rateSliderAttachment1(audioProcessor.apvts, "Band1.Rate", rateSlider1),
    rateSlider1(*audioProcessor.apvts.getParameter("Band1.Rate"), ""),
    centreFrequencySliderAttachment1(audioProcessor.apvts, "Band1.CentreFrequency", centreFrequencySlider1),
    centreFrequencySlider1(*audioProcessor.apvts.getParameter("Band1.CentreFrequency"), ""),
    feedbackSliderAttachment1(audioProcessor.apvts, "Band1.Feedback", feedbackSlider1),
    feedbackSlider1(*audioProcessor.apvts.getParameter("Band1.Feedback"), ""),
    mixSliderAttachment1(audioProcessor.apvts, "Band1.Mix", mixSlider1),
    mixSlider1(*audioProcessor.apvts.getParameter("Band1.Mix"), ""),
    depthSliderAttachment2(audioProcessor.apvts, "Band2.Depth", depthSlider2),
    depthSlider2(*audioProcessor.apvts.getParameter("Band2.Depth"), ""),
    rateSliderAttachment2(audioProcessor.apvts, "Band2.Rate", rateSlider2),
    rateSlider2(*audioProcessor.apvts.getParameter("Band2.Rate"), ""),
    centreFrequencySliderAttachment2(audioProcessor.apvts, "Band2.CentreFrequency", centreFrequencySlider2),
    centreFrequencySlider2(*audioProcessor.apvts.getParameter("Band2.CentreFrequency"), ""),
    feedbackSliderAttachment2(audioProcessor.apvts, "Band2.Feedback", feedbackSlider2),
    feedbackSlider2(*audioProcessor.apvts.getParameter("Band2.Feedback"), ""),
    mixSliderAttachment2(audioProcessor.apvts, "Band2.Mix", mixSlider2),
    mixSlider2(*audioProcessor.apvts.getParameter("Band2.Mix"), ""),
    depthSliderAttachment3(audioProcessor.apvts, "Band3.Depth", depthSlider3),
    depthSlider3(*audioProcessor.apvts.getParameter("Band3.Depth"), ""),
    rateSliderAttachment3(audioProcessor.apvts, "Band3.Rate", rateSlider3),
    rateSlider3(*audioProcessor.apvts.getParameter("Band3.Rate"), ""),
    centreFrequencySliderAttachment3(audioProcessor.apvts, "Band3.CentreFrequency", centreFrequencySlider3),
    centreFrequencySlider3(*audioProcessor.apvts.getParameter("Band3.CentreFrequency"), ""),
    feedbackSliderAttachment3(audioProcessor.apvts, "Band3.Feedback", feedbackSlider3),
    feedbackSlider3(*audioProcessor.apvts.getParameter("Band3.Feedback"), ""),
    mixSliderAttachment3(audioProcessor.apvts, "Band3.Mix", mixSlider3),
    mixSlider3(*audioProcessor.apvts.getParameter("Band3.Mix"), ""),
    depthSliderAttachment4(audioProcessor.apvts, "Band4.Depth", depthSlider4),
    depthSlider4(*audioProcessor.apvts.getParameter("Band4.Depth"), ""),
    rateSliderAttachment4(audioProcessor.apvts, "Band4.Rate", rateSlider4),
    rateSlider4(*audioProcessor.apvts.getParameter("Band4.Rate"), ""),
    centreFrequencySliderAttachment4(audioProcessor.apvts, "Band4.CentreFrequency", centreFrequencySlider4),
    centreFrequencySlider4(*audioProcessor.apvts.getParameter("Band4.CentreFrequency"), ""),
    feedbackSliderAttachment4(audioProcessor.apvts, "Band4.Feedback", feedbackSlider4),
    feedbackSlider4(*audioProcessor.apvts.getParameter("Band4.Feedback"), ""),
    mixSliderAttachment4(audioProcessor.apvts, "Band4.Mix", mixSlider4),
    mixSlider4(*audioProcessor.apvts.getParameter("Band4.Mix"), ""),
    freqSliderAttachment1(audioProcessor.apvts, "Crossover1.freq", freqSlider1),
    freqSliderAttachment2(audioProcessor.apvts, "Crossover2.freq", freqSlider2),
    freqSliderAttachment3(audioProcessor.apvts, "Crossover3.freq", freqSlider3),
    panSliderAttachment(audioProcessor.apvts, "pan", panSlider),
    numBandsAttachment(audioProcessor.apvts, "numBands", numBandsSlider)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.


    for (auto* comp : getComps())
    {
        addAndMakeVisible(comp);
    }

    setSize(800, 600);


}

PhaserAudioProcessorEditor::~PhaserAudioProcessorEditor()
{
}

//==============================================================================
void PhaserAudioProcessorEditor::paint(juce::Graphics& g)
{

    int separatorTop = 89;
    g.setGradientFill(juce::ColourGradient(juce::Colour::fromRGB(90, 90, 90), 0, separatorTop, juce::Colour::fromRGB(60, 60, 60), 0, getHeight(), false));
    g.fillRect(juce::Rectangle<int>(0, separatorTop, getWidth(), getHeight()));

    int separatorTop2 = 140;
    g.setGradientFill(juce::ColourGradient(juce::Colour::fromRGB(90, 90, 90), 0, separatorTop2, juce::Colour::fromRGB(25, 28, 50), 0, getHeight(), false));
    g.fillRect(juce::Rectangle<int>(0, separatorTop2, getWidth(), getHeight()));
    g.setColour(juce::Colours::black);
    g.drawLine(0, 220, getWidth(), 220, 2.0f);
    juce::Path line;
    g.setColour(juce::Colours::black);
    //se introduce una linea
    line.addLineSegment(juce::Line<float>(0, separatorTop2, getWidth(), separatorTop2), 0.5f);
    g.strokePath(line, juce::PathStrokeType(0.8f, juce::PathStrokeType::mitered, juce::PathStrokeType::butt));
    line.clear();

    //TEXTO
    juce::Font f;
    juce::Font f1;
    juce::Font f2;
    juce::Font f3;
    f.setHeight(38);
    f.setBold(true);
    g.setFont(f);
    g.setColour(juce::Colour::fromRGBA(241, 241, 237, 255));
    g.drawText("Multiband Phaser", 20, 0, 300, separatorTop, juce::Justification::centredLeft);
    f1.setHeight(20);
    g.setFont(f1);
    g.drawText("CGaudio", 20, 0, 250, separatorTop, juce::Justification::bottomLeft);
    f2.setHeight(15);
    g.setFont(f2);
    g.drawText("Num Bands", 72, 100, 250, separatorTop, juce::Justification::bottomLeft);
    g.drawText("PAN", 700, 100, 250, separatorTop, juce::Justification::bottomLeft);

    int centerX = getWidth() / 2; // Obtener el centro horizontal de la interfaz
    int centerY = getHeight() / 2; // Obtener el centro vertical de la interfaz
    f2.setHeight(20);
    f2.setBold(true);
    g.setFont(f2);
    g.drawText("PARAMETERS", centerX - 125, 125, 250, separatorTop, juce::Justification::centredBottom);
    g.drawText("CROSSOVERS", centerX - 125, 0, 250, separatorTop, juce::Justification::centredBottom);

    f3.setHeight(15);
    g.setFont(f3);
    g.drawText("Depth", 15, 210, 250, separatorTop, juce::Justification::bottomLeft);
    g.drawText("Rate", 15, 280, 250, separatorTop, juce::Justification::bottomLeft);
    g.drawText("Feedback", 0, 350, 250, separatorTop, juce::Justification::bottomLeft);
    g.drawText("Mix", 15, 485, 250, separatorTop, juce::Justification::bottomLeft);
    g.drawFittedText("Centre\nFreq", 5, 420, 250, separatorTop, juce::Justification::bottomLeft, 2);
    //imagen

    juce::File desktopDir = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);
    juce::File imageFile = desktopDir.getChildFile("logo_etsist32.png");
    juce::Image image;
    if (imageFile.existsAsFile())
    {
        image = juce::ImageFileFormat::loadFrom(imageFile);
    }
    else
    {
        // Si la imagen no existe, puedes manejar el caso de error aqu�
    }

    // Ahora puedes usar la imagen cargada en tu c�digo, por ejemplo:
    g.drawImage(image, juce::Rectangle<float>(720, 10, 60, 60), juce::RectanglePlacement::fillDestination);




}

void PhaserAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
        // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto infoArea = bounds.removeFromTop(bounds.getHeight() * 0.15);
    auto freqArea = bounds.removeFromTop(bounds.getHeight() * 0.10);
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.20);

    auto firstBandArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
    auto secondBandArea = bounds.removeFromLeft(bounds.getWidth() * 0.333);
    auto thirdBandArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto fourthBandArea = bounds;

    // Posicionar freqSlider1
    freqSlider1.setBounds(freqArea.removeFromLeft(freqArea.getWidth() * 0.33));

    // Calcular el ancho de los sliders restantes
    int sliderWidth = freqArea.getWidth() / 2;

    // Posicionar freqSlider2
    freqSlider2.setBounds(freqArea.removeFromLeft(sliderWidth));

    // Posicionar freqSlider3
    freqSlider3.setBounds(freqArea);

    //PAN
    juce::Rectangle<int> panSliderBounds = responseArea.removeFromRight(sliderWidth);
    panSliderBounds.setWidth(panSliderBounds.getWidth() * 0.8); // Ajusta el ancho a un 80% del valor original
    panSliderBounds.setHeight(panSliderBounds.getHeight() * 0.8); // Ajusta el alto a un 80% del valor original
    panSlider.setBounds(panSliderBounds);

    //NUM BANDS


    juce::Rectangle<int> numBandsSliderBounds = responseArea.removeFromLeft(sliderWidth);
    numBandsSliderBounds.setWidth(numBandsSliderBounds.getWidth() * 0.8); // Ajusta el ancho a un 80% del valor original
    numBandsSliderBounds.setHeight(numBandsSliderBounds.getHeight() * 0.8); // Ajusta el alto a un 80% del valor original
    numBandsSlider.setBounds(numBandsSliderBounds);
    /*=========================================================================*/
        /*
        depthSlider1.setBounds(firstBandArea.removeFromTop(firstBandArea.getHeight() * 0.25 * 0.7));//cuarto
        rateSlider1.setBounds(firstBandArea.removeFromTop(firstBandArea.getHeight() * 0.33 * 0.65));//tercio del restante
        feedbackSlider1.setBounds(firstBandArea.removeFromTop(firstBandArea.getHeight() * 0.5 * 0.5));//mitad del restante
        centreFrequencySlider1.setBounds(firstBandArea.removeFromTop(firstBandArea.getHeight() * 0.4));
         */
    float sliderHeight = firstBandArea.getHeight() / 4.0f; // Dividir en 4 partes iguales

    depthSlider1.setBounds(firstBandArea.removeFromTop(sliderHeight * 0.75));
    rateSlider1.setBounds(firstBandArea.removeFromTop(sliderHeight * 0.75));
    feedbackSlider1.setBounds(firstBandArea.removeFromTop(sliderHeight * 0.75));
    centreFrequencySlider1.setBounds(firstBandArea.removeFromTop(sliderHeight * 0.75));
    mixSlider1.setBounds(firstBandArea.removeFromTop(sliderHeight * 0.75));
    /*=========================================================================*/
    depthSlider2.setBounds(secondBandArea.removeFromTop(sliderHeight * 0.75));
    rateSlider2.setBounds(secondBandArea.removeFromTop(sliderHeight * 0.75));
    feedbackSlider2.setBounds(secondBandArea.removeFromTop(sliderHeight * 0.75));
    centreFrequencySlider2.setBounds(secondBandArea.removeFromTop(sliderHeight * 0.75));
    mixSlider2.setBounds(secondBandArea.removeFromTop(sliderHeight * 0.75));
    /*=========================================================================*/
    depthSlider3.setBounds(thirdBandArea.removeFromTop(sliderHeight * 0.75));
    rateSlider3.setBounds(thirdBandArea.removeFromTop(sliderHeight * 0.75));
    feedbackSlider3.setBounds(thirdBandArea.removeFromTop(sliderHeight * 0.75));
    centreFrequencySlider3.setBounds(thirdBandArea.removeFromTop(sliderHeight * 0.75));
    mixSlider3.setBounds(thirdBandArea.removeFromTop(sliderHeight * 0.75));
    /*=========================================================================*/
    depthSlider4.setBounds(fourthBandArea.removeFromTop(sliderHeight * 0.75));
    rateSlider4.setBounds(fourthBandArea.removeFromTop(sliderHeight * 0.75));
    feedbackSlider4.setBounds(fourthBandArea.removeFromTop(sliderHeight * 0.75));
    centreFrequencySlider4.setBounds(fourthBandArea.removeFromTop(sliderHeight * 0.75));
    mixSlider4.setBounds(fourthBandArea.removeFromTop(sliderHeight * 0.75));

}





std::vector<juce::Component*> PhaserAudioProcessorEditor::getComps()
{
    return
    {
        &rateSlider1,
        &depthSlider1,
        &feedbackSlider1,
        &centreFrequencySlider1,
        &mixSlider1,
        &rateSlider2,
        &depthSlider2,
        &feedbackSlider2,
        &centreFrequencySlider2,
        &mixSlider2,
        &rateSlider3,
        &depthSlider3,
        &feedbackSlider3,
        &centreFrequencySlider3,
        &mixSlider3,
        &rateSlider4,
        &depthSlider4,
        &feedbackSlider4,
        &centreFrequencySlider4,
        &mixSlider4,
        &freqSlider1,
        &freqSlider2,
        &freqSlider3,
        &panSlider,
        &numBandsSlider
    };
}

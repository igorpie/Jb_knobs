#pragma once
#include <JuceHeader.h>
#define M_PI		3.14159265358979323846f
#define jcolor  juce::Colours::white

class AfxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AfxLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::lightskyblue);
        setColour(juce::Slider::textBoxTextColourId , juce::Colours::darkgrey);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;

        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;

        // background circle fill
        g.setColour(juce::Colours::black);
        g.fillEllipse(rx, ry, rw, rw);

        // outer circle 
        g.setColour(jcolor);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);

        // pointer
        juce::Path p;
        auto pointerLength = radius * 0.47f;
        auto pointerThickness = radius/10;

        p.addRoundedRectangle(-pointerThickness, -pointerLength, pointerThickness, pointerLength , pointerThickness/2 , pointerThickness/2);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX + pointerThickness /2, centreY));
        g.setColour(juce::Colours::white);
        g.fillPath(p);

        juce::Path p2;
        auto dotsize = radius / 10;
        p2.addEllipse(-dotsize / 2, -radius * 0.95f, dotsize, dotsize); // float x, float y, float w, float h
        p2.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX , centreY));
        g.fillPath(p2);


        // 7 edges
        float jangle_step = 2.f * M_PI / 7.f;
        float jangle = angle + (M_PI + jangle_step )/2;
        float jr = radius * 0.8f;
        float jx, jx1, jy, jy1;
        g.setColour(jcolor);
        jx = jr * cosf(jangle) + centreX; jy = jr * sinf(jangle) + centreY;

        for (int i = 0; i < 7; i++) {
            jangle += jangle_step;
            jx1 = jr * cosf(jangle) + centreX ;
            jy1 = jr * sinf(jangle) + centreY;
            g.drawLine( jx, jy, jx1, jy1 );
            jx = jx1;
            jy = jy1;
        }

        // 7 arcs
        g.setColour(juce::Colours::red);
        jr = radius * 2;
        jangle = angle + (M_PI + jangle_step) / 2;
        jx = jr * cosf(jangle) + centreX; jy = jr * sinf(jangle) + centreY;
        juce::Path p3;
        for (int i = 0; i < 7; i++) {
            jangle += jangle_step;
            jx1 = -jr * cosf(jangle) + centreX;
            jy1 = -jr * sinf(jangle) + centreY;
            p3.addCentredArc(jx1, jy1, jr * 0.666f , jr * 0.666f, jangle + jangle_step + M_PI / 2, -jangle_step * 0.707f, -jangle_step - M_PI * 0.08f, true); 
            jx = jx1;
            jy = jy1;
        }
        g.setColour(jcolor);
        g.strokePath(p3 , juce::PathStrokeType(1.0));
    }
};



class GenericEditor : public juce::AudioProcessorEditor
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    GenericEditor(juce::AudioProcessor& parent, juce::AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor(parent),
        valueTreeState(vts) 
    {
        setResizable(true, true);
        setSize(400, 300);
        
        addAndMakeVisible(volLabel);
        volLabel.setText("VOLUME", juce::dontSendNotification);
        volLabel.setJustificationType(juce::Justification::centred);

        addAndMakeVisible(volSlider);
        volSlider.setLookAndFeel(&afxLookAndFeel);
        volSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, volSlider.getTextBoxHeight());
        volAttachment.reset(new SliderAttachment(valueTreeState, "vol", volSlider));
    }
    

    void resized() override
    {
        float x = (float)getWidth();
        float kW = 400 / x;
        float y = (float)getHeight();
        float kH = 300 / y;

        int dx = (int)(120.f / kW);
        int col2 = 20 + dx;
        volLabel.setBounds(col2, (int)(170.f / kH), dx, (int)( 20.f / kH) );
        volSlider.setBounds(col2, 30, dx, (int)(140.f / kH) );
    }

    void paint(juce::Graphics& g) override {
        g.fillAll((const Colour)0xFF000030);
    }
    
private:
    AfxLookAndFeel afxLookAndFeel;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Label  volLabel;
    juce::Slider volSlider;
    std::unique_ptr<SliderAttachment> volAttachment;
};



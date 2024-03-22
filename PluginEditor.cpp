/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainPitchAudioProcessorEditor::GainPitchAudioProcessorEditor (GainPitchAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 20);
    gainSlider.setRange(-60.0f, 0.0f, 0.01f);
    gainSlider.setValue(-20.0f);
    gainSlider.setTextValueSuffix (" dB");
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    
    addAndMakeVisible (gainLabel);
    gainLabel.setText ("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent (&gainSlider, false);
    
    
    pitchSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 150, 20);
    pitchSlider.setRange(0.0f, 1.0f, 1.0f);
    pitchSlider.setValue(0.0f);
    pitchSlider.setTextValueSuffix ("[ON/OFF]");
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);
    
    addAndMakeVisible (pitchLabel);
    pitchLabel.setText ("Pitch", juce::dontSendNotification);
    pitchLabel.attachToComponent (&pitchSlider, false);
    
    setSize (400, 300);
}

GainPitchAudioProcessorEditor::~GainPitchAudioProcessorEditor()
{
}

//==============================================================================
void GainPitchAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    gainSlider.setBounds(50, getHeight() / 2 - 100, 100, 200);
    pitchSlider.setBounds(250, getHeight() / 2 - 100, 100, 200);
}

void GainPitchAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void GainPitchAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
    if (slider == &gainSlider) {
        audioProcessor.rawVolume = gainSlider.getValue();
    }
    
    if (slider == &pitchSlider){
        audioProcessor.pitchValue = pitchSlider.getValue();
    }
}


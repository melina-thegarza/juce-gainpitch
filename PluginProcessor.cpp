/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainPitchAudioProcessor::GainPitchAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GainPitchAudioProcessor::~GainPitchAudioProcessor()
{
}

//==============================================================================
const juce::String GainPitchAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainPitchAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GainPitchAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GainPitchAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GainPitchAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainPitchAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GainPitchAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainPitchAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GainPitchAudioProcessor::getProgramName (int index)
{
    return {};
}

void GainPitchAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GainPitchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GainPitchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainPitchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GainPitchAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Create a temporary buffer to store the processed audio
    juce::AudioBuffer<float> tempBuffer(buffer.getNumChannels(), numSamples);
    tempBuffer.clear();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* tempChannelData = tempBuffer.getWritePointer(channel);
        

        // ..do something to the data...
        //go through all of our samples in our buffer
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
            channelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(rawVolume);
        }
//        drop every other sample
        if (pitchValue == 1.0){
            for (int sample = 0; sample < buffer.getNumSamples(); sample += 2){ // Increment by 2 to process every other sample
                    channelData[sample] = 0.0f; // Set the sample to zero
            }
            for (int sample = 0; sample < numSamples; ++sample)
            {
                    tempChannelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(rawVolume);
            }
        }
    }
    
    if (pitchValue == 1.0){
        // Concatenate the original buffer with the temporary buffer (playback twice)
            buffer.setSize(totalNumInputChannels, numSamples * 2, true, true, true);
            buffer.copyFrom(0, numSamples, tempBuffer, 0, 0, numSamples);
    }
    
}

//==============================================================================
bool GainPitchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainPitchAudioProcessor::createEditor()
{
    return new GainPitchAudioProcessorEditor (*this);
}

//==============================================================================
void GainPitchAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GainPitchAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainPitchAudioProcessor();
}

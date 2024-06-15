/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>

//==============================================================================
/**
*/
class ADS_KobolAudioProcessor  : public juce::AudioProcessor{
public:
    //==============================================================================
    ADS_KobolAudioProcessor();
    ~ADS_KobolAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Getters and setters for ADSR parameters
    juce::ADSR::Parameters getAdsrParams() const { return adsrParams; }
    void setAdsrParams(const juce::ADSR::Parameters& params);

    // Functions for load and play samples
    void loadSample(const juce::File& file); 
    void playSample();

    // Exponential functions voltage to seconds
    float attackCurve(float v);
    float decayCurve(float v);
    float sustainCurve(float v);

    // noteOn and noteOf for call the ads
    void noteOn();
    void noteOff();

    // Recorder
    void startRecording();
    void stopRecording();
    bool isRecording() const { return recording; }

   


private:
    //==============================================================================
    
    //ADSR de JUCE
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    float envValue;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    // ADS Sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;

    float attackSliderValue;
    float decaySliderValue;
    float sustainSliderValue;
    float releaseSliderValue;


    // Recorder 
    juce::TimeSliceThread thread{ "Audio Recorder Thread" };
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    juce::ScopedPointer<juce::AudioFormatWriter> writer;
    bool recording = false;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADS_KobolAudioProcessor)


};

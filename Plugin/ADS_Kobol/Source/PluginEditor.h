#pragma once

#include <JuceHeader.h>
#include "Visualizer.h"
#include "PluginProcessor.h"

class ADS_KobolAudioProcessorEditor : public juce::AudioProcessorEditor, 
                                      public juce::Button::Listener
                                      
{
public:
    ADS_KobolAudioProcessorEditor(ADS_KobolAudioProcessor&);
    ~ADS_KobolAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    std::unique_ptr<juce::FileChooser> fileChooser;
    Visualizer* getVisualizer() { return &visualizer; }; // Visualizer



private:
    ADS_KobolAudioProcessor& audioProcessor;

    // ADS Sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;

    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;


    // Load and play buttons 
    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ "Play" };
    juce::Label filenameLabel;
    juce::File previousDirectory;


    // Record button
    juce::TextButton recordButton;

    // Min and max voltage
    const float minVoltage = 0.0f;
    const float maxVoltage = 10.0f;

    // Visualizer
    Visualizer visualizer;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADS_KobolAudioProcessorEditor)
};
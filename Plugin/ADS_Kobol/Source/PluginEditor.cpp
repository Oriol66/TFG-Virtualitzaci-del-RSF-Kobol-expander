/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>
#include "Visualizer.h"

//==============================================================================
ADS_KobolAudioProcessorEditor::ADS_KobolAudioProcessorEditor (ADS_KobolAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), visualizer()
{
    previousDirectory = juce::File::getSpecialLocation(juce::File::userHomeDirectory);

    fileChooser = std::make_unique<juce::FileChooser>("Choose file", previousDirectory, "*.wav", true, false, this);

    
    // Sliders for ADS
    attackSlider.setRange(minVoltage, maxVoltage);
    decaySlider.setRange(minVoltage, maxVoltage);
    sustainSlider.setRange(minVoltage, maxVoltage);

    // Map Voltage to seconds for each slider
    attackSlider.onValueChange = [this] {

        float attackVoltage = attackSlider.getValue();
        float attackSeconds = audioProcessor.attackCurve(attackVoltage);
        auto params = audioProcessor.getAdsrParams();
        params.attack = attackSeconds;
        audioProcessor.setAdsrParams(params);

        };

    decaySlider.onValueChange = [this] {
        float decayVoltage = decaySlider.getValue();
        float decaySeconds = audioProcessor.decayCurve(decayVoltage);
        auto params = audioProcessor.getAdsrParams();
        params.decay = decaySeconds;
        audioProcessor.setAdsrParams(params);

        };

    sustainSlider.onValueChange = [this] {
        float sustainVoltage = sustainSlider.getValue();
        float sustainAmplitude = audioProcessor.sustainCurve(sustainVoltage);
        auto params = audioProcessor.getAdsrParams();
        params.sustain = sustainAmplitude;
        audioProcessor.setAdsrParams(params);
        };
    
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(decaySlider);
    addAndMakeVisible(sustainSlider);

    attackLabel.setText("Attack", juce::dontSendNotification);
    decayLabel.setText("Decay", juce::dontSendNotification);
    sustainLabel.setText("Sustain", juce::dontSendNotification);


    addAndMakeVisible(attackLabel);
    addAndMakeVisible(decayLabel);
    addAndMakeVisible(sustainLabel);

    
    //Load and play buttons
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);

    addAndMakeVisible(playButton);
    playButton.addListener(this);

    // Make the file name visible
    addAndMakeVisible(filenameLabel);
    filenameLabel.setJustificationType(juce::Justification::centred);
    filenameLabel.setText("No file selected", juce::dontSendNotification);

    // Record Button
    recordButton.setButtonText("Start Recording");
    recordButton.addListener(this);
    addAndMakeVisible(&recordButton);

    //Visualizer
    addAndMakeVisible(visualizer);

    
    setSize(800, 500);
}

ADS_KobolAudioProcessorEditor::~ADS_KobolAudioProcessorEditor()
{
    //recordButton.removeListener(this);
}

//==============================================================================
void ADS_KobolAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    /*
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    */
}

void ADS_KobolAudioProcessorEditor::resized()
{

    // Sliders and Labels
    int sliderWidth = getWidth() - 180;
    int sliderHeight = 30;
    int sliderX = 80;

    attackSlider.setBounds(sliderX, 40, sliderWidth, sliderHeight);
    attackLabel.setBounds(getWidth()/2 - 10, 30, 60, 20);

    decaySlider.setBounds(sliderX, 80, sliderWidth, sliderHeight);
    decayLabel.setBounds(getWidth() / 2 - 10, 70, 60, 20);

    sustainSlider.setBounds(sliderX, 120, sliderWidth, sliderHeight);
    sustainLabel.setBounds(getWidth() / 2 - 10 ,110, 60, 20);

    // Buttons
    int buttonHeight = 30;
    int buttonY = getHeight() - buttonHeight - 20;
    int buttonWidth = (getWidth() - 60) / 3;

    loadButton.setBounds(15, buttonY, buttonWidth, buttonHeight);
    playButton.setBounds(30 + buttonWidth, buttonY, buttonWidth, buttonHeight);
    recordButton.setBounds(45 + buttonWidth*2, buttonY, buttonWidth, buttonHeight);

    // Loaded file
    filenameLabel.setBounds(20, 400, getWidth() - 40, 20);

    // Visualizer
    int visualizerHeight = 200;
    int visualizerY = 160;
    visualizer.setBounds(20, visualizerY, getWidth() - 40, visualizerHeight);
    
}

//================================================================================================


// Load and play Buttons
void ADS_KobolAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {

        // Create file chooser with default directory
        juce::FileChooser chooser("Select a Wave file to play...", previousDirectory, "*.wav");

        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();

                if (file.existsAsFile())
                {
                    filenameLabel.setText("File selected: " + file.getFileName(), juce::dontSendNotification);
                    DBG("File selected: " + file.getFullPathName());
                    audioProcessor.loadSample(file);
                    previousDirectory = file.getParentDirectory();  //Get parent directory
                    fileChooser = std::make_unique<juce::FileChooser>("Choose file", previousDirectory, "*.wav");

                }
                else
                {
                    DBG("No file selected or file does not exist.");
                }
            });
    } 

    if (button == &recordButton)
    {
        if (audioProcessor.isRecording())
        {
            audioProcessor.stopRecording();
            recordButton.setButtonText("Start Recording");
        }
        else
        {
            audioProcessor.startRecording();
            recordButton.setButtonText("Stop Recording");
        }
    }

    if (button == &playButton)
    {

        audioProcessor.playSample();
    }
}



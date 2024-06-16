/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <JuceHeader.h>
#include <juce_audio_utils/juce_audio_utils.h>


using namespace juce;
//==============================================================================
ADS_KobolAudioProcessor::ADS_KobolAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{   
    formatManager.registerBasicFormats();
    thread.startThread();
    adsrParams.attack = 0.0001;
    adsrParams.decay = 0.0001;
    adsrParams.sustain = 0.0001;
    adsr.setParameters(adsrParams);
}

ADS_KobolAudioProcessor::~ADS_KobolAudioProcessor()
{
    /*
    thread.stopThread(1000);
    readerSource.reset();
    transportSource.setSource(nullptr);
    transportSource.releaseResources(); 
    adsr.reset();
    */

}

//==============================================================================


float ADS_KobolAudioProcessor::attackCurve(float v)
{
    float a = 0.000259;
    float b = 1.089287;
    float seconds = a * std::exp(b * v);
    return seconds; // Funció exponencial pel Attack
}

float ADS_KobolAudioProcessor::decayCurve(float v)
{
    float a = 0.002562f;
    float b = 0.750869f;
    return a * std::exp(b * v); // Funció exponencial pel Attack
}

float ADS_KobolAudioProcessor::sustainCurve(float v)
{
    float a0 = -0.239039f;
    float a1 = 0.123904f;
    float y = a0 + a1 * v;
    
    if (y < 0) {
        y = 0;
    }
    else if (y > 1) {
        y = 1;
    }

    return y; // Funció de amplitud del Sustain
}


//==============================================================================

const juce::String ADS_KobolAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ADS_KobolAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ADS_KobolAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ADS_KobolAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ADS_KobolAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ADS_KobolAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ADS_KobolAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ADS_KobolAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ADS_KobolAudioProcessor::getProgramName (int index)
{
    return {};
}

void ADS_KobolAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void ADS_KobolAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    transportSource.prepareToPlay(samplesPerBlock, sampleRate);
    adsr.setSampleRate(sampleRate);
    
    adsr.setSampleRate(sampleRate);

}

void ADS_KobolAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    transportSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ADS_KobolAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ADS_KobolAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

    }

    buffer.clear();

    if (transportSource.isPlaying()) {
        transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));        

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                float envValue = adsr.getNextSample();
                channelData[sample] *= envValue;  // Apply envelope value
            }
        }
    }

    
    //Recording
    if (recording && threadedWriter != nullptr)
    {
        threadedWriter->write(buffer.getArrayOfReadPointers(), buffer.getNumSamples());
    }
}

//==============================================================================
bool ADS_KobolAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ADS_KobolAudioProcessor::createEditor()
{
    return new ADS_KobolAudioProcessorEditor (*this);
}

//==============================================================================
void ADS_KobolAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ADS_KobolAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ADS_KobolAudioProcessor();
}


//Load Sample
void ADS_KobolAudioProcessor::loadSample(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

void ADS_KobolAudioProcessor::playSample()
{
    if (readerSource.get() != nullptr)
    {
        transportSource.setPosition(0.0);
        transportSource.start();
        adsr.noteOn();  // Activate ADSR
        
    }
    else
    {
        DBG("Reader source is null, cannot start playback.");
    }
}

void ADS_KobolAudioProcessor::noteOn()
{
    DBG("ADSR note on triggered");
    adsr.noteOn();
}

void ADS_KobolAudioProcessor::noteOff()
{
    DBG("ADSR note off triggered");
    adsr.noteOff();
}

//========================================================================================
// Recorder

void ADS_KobolAudioProcessor::startRecording()
{
    
    juce::File file = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("recorded_audio.wav");
    auto fileStream = std::make_unique<juce::FileOutputStream>(file);

    if (fileStream != nullptr && fileStream->openedOk())
    {
        juce::WavAudioFormat wavFormat;
        writer.reset(wavFormat.createWriterFor(fileStream.get(),
            getSampleRate(),
            getTotalNumOutputChannels(),
            16, // bits per sample
            {}, // metadata
            0)); // quality option index

        // Transfer ownership of fileStream to the writer if it was successfully created
        if (writer != nullptr)
        {
            // Release the unique_ptr to avoid double-deletion
            fileStream.release();

            threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer.get(), thread, 32768));
            recording = true;
        }
        else
        {
            DBG("Error creating audio format writer");
        }
    }
    else
    {
        DBG("Error creating file output stream or opening the file stream");
    }
}

void ADS_KobolAudioProcessor::stopRecording()
{
    if (recording)
    {
        recording = false;
        threadedWriter.reset(); //Reset
        DBG("Stopping recording and launching file chooser");

        // FileChoser for save the file
        juce::FileChooser fileChooser("Save Recorded Audio",
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
            "*.wav");

        auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

        // Use launchAsync for FileChooser
        fileChooser.launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
            {
                juce::File chosenFile = fc.getResult();

                if (chosenFile != juce::File())
                {
                    DBG("File chosen: " + chosenFile.getFullPathName());

                    // Make sure direction is possible
                    juce::File tempFile = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("recorded_audio.wav");

                    // Move temporal file to direc
                    if (tempFile.existsAsFile())
                    {
                        tempFile.moveFileTo(chosenFile);
                        DBG("File moved to: " + chosenFile.getFullPathName());
                    }
                    else
                    {
                        DBG("Error: Temporary file does not exist");
                    }
                }
                else
                {
                    DBG("User cancelled the file save operation");
                }

                // Reset for close correctly
                writer.reset();
            });
    }
    else
    {
        DBG("Recording was not active");
    }
}


void ADS_KobolAudioProcessor::setAdsrParams(const juce::ADSR::Parameters& params) {
    adsrParams = params;
    adsr.setParameters(params);
}
#ifndef h_Drums
#define h_Drums

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

class Drums : public AudioSource
{
public:
    Drums();
    ~Drums();
    void NoteOn(int note, float velocity);
    void clear();
    
    // AudioSoure overrides
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    static Drums& instance(void)
    {
        static Drums s_instance;
        return s_instance;
    }
    
    MidiKeyboardState playbackState;
    bool recording = true;
    bool metronomeOn = true;
    
private:
    MidiKeyboardState keyboardState;
    Synthesiser synth;
    MidiMessageCollector midiCollector;
    MidiBuffer recordBuffer;
    MidiBuffer metronomeBuffer;
    long sampleCounter = 0;
    long maxRecordSamples = 0;
    float tempo = 110;
    CriticalSection midiBufferLock;
};

#endif

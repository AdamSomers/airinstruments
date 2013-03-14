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
    void clearTrack(int note);
    
    // AudioSoure overrides
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    const int getNumNotes() const { return numNotes; }
    
    static Drums& instance(void)
    {
        static Drums s_instance;
        return s_instance;
    }
    
    MidiKeyboardState playbackState;
    bool recording;
    bool metronomeOn;
    
private:
    MidiKeyboardState keyboardState;
    Synthesiser synth;
    MidiMessageCollector midiCollector;
    MidiBuffer recordBuffer;
    MidiBuffer metronomeBuffer;
    long sampleCounter;
    long maxRecordSamples;
    float tempo;
    int numNotes;
    CriticalSection midiBufferLock;
};

#endif

#ifndef h_Drums
#define h_Drums

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

#include "DrumPattern.h"
#include "DrumKit.h"


class Drums : public AudioSource
{
public:
    Drums();
    ~Drums();
    void NoteOn(int note, float velocity);
    void clear();
    void clearTrack(int note);
    
    // AudioSource overrides
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    const int getNumNotes() const { return numNotes; }
	double getSampleRate() const { return sampleRate; }
	SharedPtr<DrumKit> getDrumKit() const { return kit; }
	SharedPtr<DrumPattern> getPattern() const { return pattern; }
    
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
    SharedPtr<DrumPattern> pattern;
    MidiBuffer metronomeBuffer;
    long sampleCounter;
    long maxRecordSamples;
    float tempo;
    int numNotes;
    double sampleRate;
    CriticalSection midiBufferLock;
	SharedPtr<DrumKit> kit;
};

#endif

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
    void resetToZero();
    
    // AudioSource overrides
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    const int getNumNotes() const { return numNotes; }
	double getSampleRate() const { return sampleRate; }
	SharedPtr<DrumKit> getDrumKit() const { return kit; }
	SharedPtr<DrumPattern> getPattern() const { return pattern; }
	void setDrumKit(SharedPtr<DrumKit> aKit, bool doLock = true);
	void setPattern(SharedPtr<DrumPattern> aPattern);
    
    static Drums& instance(void)
    {
        static Drums s_instance;
        return s_instance;
    }
    
    MidiKeyboardState playbackState;

    class TransportState : public ChangeBroadcaster
    {
    public:
        TransportState(bool record, bool play, bool metronome);
    
        void play();
        void pause();
        void record(bool state);
        void metronome(bool state);

        void toggleRecording();
        void togglePlayback();
        void toggleMetronome();

        bool recording;
        bool playing;
        bool metronomeOn;
    };

    void addTransportListener(ChangeListener* listener);
    void removeTransportListener(ChangeListener* listener);
    TransportState& getTransportState() { return transportState; }
    
private:
    TransportState transportState;
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

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
    void resetToZero();
    
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
    MidiBuffer recordBuffer;
    MidiBuffer metronomeBuffer;
    long sampleCounter;
    long maxRecordSamples;
    float tempo;
    int numNotes;
    double sampleRate;
    CriticalSection midiBufferLock;
};

#endif

#ifndef h_Drums
#define h_Drums

#include <iostream>

#include "DrumPattern.h"
#include "DrumKit.h"
#include "DrumSampler.h"

#include "../JuceLibraryCode/JuceHeader.h"


class Drums :	public AudioSource,
				public Slider::Listener
{
public:
	enum TempoSource
	{
		kGlobalTempo = 0,
		kPatternTempo = 1
	};

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

	// Slider::Listener overrides
	void sliderValueChanged (Slider *slider);

    const int getNumNotes() const { return numNotes; }
	double getSampleRate() const { return sampleRate; }
	SharedPtr<DrumKit> getDrumKit() const { return kit; }
	SharedPtr<DrumPattern> getPattern() const { return pattern; }
	void setDrumKit(SharedPtr<DrumKit> aKit);
	void setPattern(SharedPtr<DrumPattern> aPattern);
	float getTempo(void);
	void setTempo(float tempo);
	TempoSource getTempoSource(void);
	void setTempoSource(TempoSource source);
	void registerTempoSlider(Slider* slider);
    
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
	void AdjustMidiBuffers(void);	// Moves events in the midi buffers due to changes in sample rate or tempo
	void setTempoSlider(float tempo);

    TransportState transportState;
    MidiKeyboardState keyboardState;
    DrumSampler synth;
    MidiMessageCollector midiCollector;
    SharedPtr<DrumPattern> pattern;
    MidiBuffer metronomeBuffer;
    long sampleCounter;
    long maxRecordSamples;
    float globalTempo;
	TempoSource tempoSource;
    int numNotes;
    double sampleRate;
    CriticalSection midiBufferLock;
	SharedPtr<DrumKit> kit;
	Slider* tempoSlider;
};

#endif

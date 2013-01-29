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
    
    // AudioSoure overrides
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    static Drums& instance(void)
    {
        static Drums s_instance;
        return s_instance;
    }

private:
    Synthesiser synth;
    MidiMessageCollector midiCollector;
    MidiKeyboardState keyboardState;
};

#endif

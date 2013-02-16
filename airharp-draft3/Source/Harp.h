#ifndef __AirHarp__Harp__
#define __AirHarp__Harp__

#include <iostream>

//#include "MidiServer.h"
#include "Voices.h"
#include "SignalGenerators.h"
#include "JuceReverbAudioClient.h"
#include "Leap.h"

// Pentatonic Major
const std::vector<std::string> gPentatonicMajor = { "1", "2", "4", "5", "6" };

// Pentatonic Minor
const std::vector<std::string> gPentatonicMinor = {"1", "b3", "4", "5", "b7" };

// Whole-tone
const std::vector<std::string> gWholeTone = { "1", "2", "3", "#4", "#5", "#6" };

// Diatonic
const std::vector<std::string> gDiatonic = { "1", "2", "3", "4", "5", "6", "7" };

// Minor
const std::vector<std::string> gMinor = { "1", "2", "b3", "4", "5", "b6", "b7" };

// Slendro
const std::vector<std::string> gExotic1 = { "1", "3", "4", "5", "7"};

// Chinese mystery
const std::vector<std::string> gExotic2 = { "1", "3", "#4", "5", "7" };

class Harp : public Leap::Listener
{
public:
    Harp();
    ~Harp();
    void NoteOn(int num, int note, int velocity);
    void ExciteString(int num, int note, int velocity, float* buffer, int bufferSize);
    void AddString();
    void RemoveString();
    int GetNumStrings() const { return numStrings; }
    float getWetLevel() const { return wetLevel; }
    float getDryLevel() const { return dryLevel; }
    void setWetLevel(float val);
    void setDryLevel(float val);
    
    // Leap Listener override
    void onFrame(const Leap::Controller&);
    
    static Harp& instance( void )
    {
        static Harp s_instance;
        return s_instance;
    }
    
    std::vector<SampleAccumulator*>& GetBuffers() { return accumulators; }
    std::vector<Karplus*>& GetStrings() { return strings; }
    void SetScale(int scaleIndex);
    
    static std::vector<std::string> gScale;
private:
    void Cleanup();
    void Init();
    
    std::vector<SampleAccumulator*> accumulators;
    std::vector<StateVariable*> filters;
    std::vector<Karplus*> strings;
    Adder* mixer;
    Multiplier* outputGain;
    JuceReverbAudioClient* reverb;
    Multiplier* reverbGain;
    Multiplier* dryGain;
    Adder* wetDryMix;
    StateVariable* filter;
    int numStrings;
    float wetLevel;
    float dryLevel;
};

#endif /* defined(__AirHarp__Harp__) */

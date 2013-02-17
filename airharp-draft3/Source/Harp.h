#ifndef __AirHarp__Harp__
#define __AirHarp__Harp__

#include <iostream>

//#include "MidiServer.h"
#include "Voices.h"
#include "SignalGenerators.h"
#include "JuceReverbAudioClient.h"
#include "Leap.h"

const std::vector<std::string> gPentatonicMajor = { "1", "2", "4", "5", "6" };
const std::vector<std::string> gPentatonicMinor = {"1", "b3", "4", "5", "b7" };
const std::vector<std::string> gWholeTone = { "1", "2", "3", "#4", "#5", "#6" };
const std::vector<std::string> gDiatonic = { "1", "2", "3", "4", "5", "6", "7"};
const std::vector<std::string> gMinor = { "1", "2", "b3", "4", "5", "b6", "b7" };
// Slendro?
const std::vector<std::string> gExotic1 = { "1", "3", "4", "5", "7"};
// Chinese mystery
const std::vector<std::string> gExotic2 = { "1", "3", "#4", "5", "7" };

const std::vector<std::string> I     = { "1", "3", "5" };
const std::vector<std::string> ii    = { "2", "4", "6" };
const std::vector<std::string> iii   = { "3", "5", "7" };
const std::vector<std::string> IV    = { "1", "4", "6" };
const std::vector<std::string> V     = { "2", "5", "7" };
const std::vector<std::string> vi    = { "1", "3", "6" };
const std::vector<std::string> VII   = { "2", "4", "b7" };
const std::vector<std::string> vii_d = { "2", "4", "7" };

const std::vector<std::string> i     = { "1", "b3", "5" };
const std::vector<std::string> ii_d  = { "2", "4", "b6" };
const std::vector<std::string> III   = { "b3", "5", "b7" };
const std::vector<std::string> iv    = { "1", "4", "b6" };
const std::vector<std::string> v     = { "2", "5", "b7" };
const std::vector<std::string> VI    = { "1", "b3", "b6" };

const std::vector<std::string> gDiminishedChord = { "1", "b3", "b5" };

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
    
    class Lock
    {
    public:
        Lock() { lock.enter(); }
        ~Lock() { lock.exit(); }
    private:
        CriticalSection lock;
    };
    
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
    bool chordMode = true;
};

#endif /* defined(__AirHarp__Harp__) */

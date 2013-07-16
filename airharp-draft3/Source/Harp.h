#ifndef __AirHarp__Harp__
#define __AirHarp__Harp__

#include <iostream>
#include <set>

//#include "MidiServer.h"
#include "Voices.h"
#include "SignalGenerators.h"
#include "JuceReverbAudioClient.h"
#include "Leap.h"

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
    bool isActive() const { return active; }
    void setActive(bool shouldBeActive);
    void setChordMode(bool shouldBeChordMode);
    bool getChordMode() const { return chordMode; }
    void selectChord(int chordIndex);
    void deSelectChord(int chordIndex);
    bool isChordSelected(int chordIndex) const;
    int getNumSelectedChords() const;
    void setChord(int chordIndex);
    int getSelectedScale() const { return selectedScale; }
    bool checkIdle();
    void setCustomScale(std::vector<std::string>& newCustomScale);
    int suggestedStringCount();
    void setEnabled(bool shouldBeEnabled) { enabled = shouldBeEnabled; }
    // Leap Listener override
    void onFrame(const Leap::Controller&);
    
    std::vector<SampleAccumulator*>& GetBuffers() { return accumulators; }
    std::vector<Karplus*>& GetStrings() { return strings; }
    void SetScale(int scaleIndex);
    
    std::vector<std::string>& getScale();
    
    CriticalSection lock;

private:
    void Cleanup();
    void Init();
    void BuildDefaultScales();
    
    std::vector<SampleAccumulator*> accumulators;
    std::vector<Karplus*> strings;
    Adder* mixer;
    Multiplier* outputGain;
    JuceReverbAudioClient* reverb;
    Multiplier* reverbGain;
    Multiplier* dryGain;
    Adder* wetDryMix;
    StateVariable* filter;
    StateVariable* preVerbFilter;
    int numStrings;
    float wetLevel;
    float dryLevel;
    bool active;
    bool chordMode;
    std::vector<int> selectedChords;

    typedef std::map<std::string, std::vector<std::string> > ScaleMap;
    ScaleMap scales;
    
    std::string selectedScaleName;
    int selectedScale;
    bool idle;
    bool enabled;
};

class HarpManager
{
public:
    
    HarpManager();
    ~HarpManager();

    static HarpManager& instance( void )
    {
        static HarpManager s_instance;
        return s_instance;
    }
    
    int getNumHarps() const { return harps.size(); }
    int getNumActiveHarps();
    Harp* getHarp(int harpIndex);
    
private:
    void activateHarp(int harpIndex);
    void deActivateHarp(int harpIndex);
    
    std::vector<Harp*> harps;
    static const int numHarps = 1;

};

#endif /* defined(__AirHarp__Harp__) */

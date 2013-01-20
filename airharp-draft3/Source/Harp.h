#ifndef __AirHarp__Harp__
#define __AirHarp__Harp__

#include <iostream>

//#include "MidiServer.h"
#include "Voices.h"
#include "SignalGenerators.h"

// Pentatonic Major
const int gPentatonicMajor[] = { 0, 2, 5, 7, 9};
const int gPentatonicMajorIntervals = 5;

// Pentatonic Minor
const int gPentatonicMinor[] = { 0, 3, 5, 7, 10};
const int gPentatonicMinorIntervals = 5;

// Whole-tone
const int gWholeTone[] = { 0, 2, 4, 6, 8, 10};
const int gWholeToneIntervals = 6;

// Diatonic
const int gDiatonic[] = { 0, 2, 4, 5, 7, 9, 11};
const int gDiatonicIntervals = 7;

class Harp
{
public:
    Harp();
    ~Harp();
    void NoteOn(int num, int note, int velocity);
    void ExciteString(int num, int note, int velocity, float* buffer, int bufferSize);
    void AddString();
    void RemoveString();
    int GetNumStrings() const { return numStrings; }
    static Harp* GetInstance();
    std::vector<SampleAccumulator*>& GetBuffers() { return accumulators; }
    std::vector<Karplus*>& GetStrings() { return strings; }
    void SetScale(int scaleIndex);
    
    static const int* gScale;
    static int gScaleIntervals;
private:
    void Cleanup();
    void Init();
    
    static Harp* sInstance;
    std::vector<SampleAccumulator*> accumulators;
    std::vector<StateVariable*> filters;
    std::vector<Karplus*> strings;
    Adder* mixer;
    Multiplier* outputGain;
    int numStrings;
};

#endif /* defined(__AirHarp__Harp__) */

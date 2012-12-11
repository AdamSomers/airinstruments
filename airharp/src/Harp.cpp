#include "Harp.h"

Harp* Harp::sInstance = NULL;

#define MAX_STRINGS 30
#define SAMPS_PER_PIXEL 6
#define FILTER_FREQ 150.f
#define FILTER_RES 0.f

Harp::Harp()
: numStrings(1)
, mixer(NULL)
, outputGain(NULL)
{
    Init();
}

Harp::~Harp()
{
    Cleanup();
}

void Harp::Init()
{
    Cleanup();
    for (int i = 0; i < numStrings; ++i)
    {
        strings.push_back(new Karplus(0.009));
        filters.push_back(new StateVariable);
        filters.back()->SetInput(strings.back());
        filters.back()->setType(StateVariable::kHighpass);
        filters.back()->setFreq(FILTER_FREQ);
        filters.back()->setRes(FILTER_RES);
        accumulators.push_back(new SampleAccumulator());
        accumulators.back()->SetInput(filters.back());
        accumulators.back()->SetSamplesPerPixel(SAMPS_PER_PIXEL);
    }
    
    mixer = new Adder;
    
    for (int i = 0; i < numStrings; ++i)
    {
        mixer->AddInput(accumulators.at(i));
    }
        
    outputGain = new Multiplier;
    outputGain->SetA(mixer);
    outputGain->SetVal(.707f);
    
    AudioServer::GetInstance()->AddClient(outputGain, 0);
    AudioServer::GetInstance()->AddClient(outputGain, 1);

}

void Harp::Cleanup()
{
    for (int i = 0; i < strings.size(); ++i)
    {
        AudioServer::GetInstance()->EnterLock();
        mixer->RemoveInput(filters.at(i));
        AudioServer::GetInstance()->ExitLock();
        delete filters.at(i);
        delete accumulators.at(i);
        delete strings.at(i);
    }
    if (outputGain) {
        delete outputGain;
        AudioServer::GetInstance()->RemoveClient(outputGain, 0);
        AudioServer::GetInstance()->RemoveClient(outputGain, 1);
    }
    if (mixer)
        delete mixer;

    strings.clear();
    accumulators.clear();
    filters.clear();
}

void Harp::AddString()
{
    if (numStrings == MAX_STRINGS )
        return;
    
    strings.push_back(new Karplus(0.009));
    filters.push_back(new StateVariable);
    filters.back()->SetInput(strings.back());
    filters.back()->setType(StateVariable::kHighpass);
    filters.back()->setFreq(FILTER_FREQ);
    filters.back()->setRes(FILTER_RES);
    accumulators.push_back(new SampleAccumulator());
    accumulators.back()->SetInput(filters.back());
    accumulators.back()->SetSamplesPerPixel(SAMPS_PER_PIXEL);
    AudioServer::GetInstance()->EnterLock();
    mixer->AddInput(accumulators.back());
    AudioServer::GetInstance()->ExitLock();
    numStrings++;
    //outputGain->SetVal(fmax(1.f/numStrings, 1/12.f));
}

void Harp::RemoveString()
{
    if (numStrings == 1)
        return;
    AudioServer::GetInstance()->EnterLock();
    mixer->RemoveInput(filters.back());
    AudioServer::GetInstance()->ExitLock();
    
    delete filters.back();
    filters.pop_back();
    delete accumulators.back();
    accumulators.pop_back();
    delete strings.back();
    strings.pop_back();
    numStrings--;
    //outputGain->SetVal(fmax(1.f/numStrings, 1/12.f));
}

void Harp::NoteOn(int num, int note, int velocity)
{
    strings.at(num)->NoteOn(note, velocity);
}

void Harp::ExciteString(int num, int note, int velocity, float* buff, int bufferSize)
{
    strings.at(num)->NoteOn(note, velocity, buff, bufferSize);
}

Harp* Harp::GetInstance()
{
    if (!sInstance)
    {
        sInstance = new Harp;
    }
    return sInstance;
}
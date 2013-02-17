#include "Harp.h"
#include "MotionServer.h"


#define MAX_STRINGS 30
#define SAMPS_PER_PIXEL 6
#define FILTER_FREQ 150.f
#define FILTER_RES 0.f

std::vector<std::string> Harp::gScale = gPentatonicMajor;

Harp::Harp()
: numStrings(1)
, mixer(NULL)
, outputGain(NULL)
, reverb(NULL)
, filter(NULL)
, reverbGain(NULL)
, dryGain(NULL)
, wetDryMix(NULL)
, wetLevel(0.3f)
, dryLevel(0.7f)
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
    
    filter = new StateVariable;
    filter->SetInput(mixer);
    filter->setFreq(1000);
    filter->setType(StateVariable::kLowpass);
    
    reverbGain = new Multiplier;
    reverbGain->SetA(filter);
    reverbGain->SetVal(wetLevel);
    
    reverb = new JuceReverbAudioClient;
    reverb->AddInput(filter);
    
    dryGain = new Multiplier;
    dryGain->SetA(mixer);
    dryGain->SetVal(0.8f);
    
    wetDryMix = new Adder;
    wetDryMix->AddInput(reverb);
    wetDryMix->AddInput(dryGain);
    
    outputGain = new Multiplier;
    outputGain->SetA(wetDryMix);
    outputGain->SetVal(dryLevel);
    
    AudioServer::GetInstance()->AddClient(outputGain, 0);
    AudioServer::GetInstance()->AddClient(outputGain, 1);
    
    MotionDispatcher::instance().controller.addListener(*this);

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
    if (filter)
        delete filter;
    
    if (reverb)
        delete reverb;
    
    if (dryGain)
        delete dryGain;
    
    if (reverbGain)
        delete reverbGain;
    
    if (wetDryMix)
        delete wetDryMix;
        
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

void Harp::SetScale(int scaleIndex)
{
    if (!chordMode)
    {
        switch (scaleIndex) {
            case 0:
                gScale = gDiatonic;
                break;
            case 1:
                gScale = gMinor;
                break;
            case 2:
                gScale = gPentatonicMajor;
                break;
            case 3:
                gScale = gPentatonicMinor;
                break;
            case 4:
                gScale = gWholeTone;
                break;
            case 5:
                gScale = gExotic1;
                break;
            case 6:
                gScale = gExotic2;
                break;
            default:
                break;
        }
    }
    else
    {
        bool minor = false;
        switch (scaleIndex) {
            case 0:
                gScale = minor ? i : I;
                break;
            case 1:
                gScale = minor ? ii_d : ii;
                break;
            case 2:
                gScale = minor ? III : iii;
                break;
            case 3:
                gScale = minor ? iv : IV;
                break;
            case 4:
                gScale = minor ? v : V;
                break;
            case 5:
                gScale = minor ? VI : vi;
                break;
            case 6:
                gScale = minor ? VII : vii_d;
                break;
            default:
                break;
        }
    }
}

void Harp::setWetLevel(float val)
{
    reverbGain->SetVal(val);
}

void Harp::setDryLevel(float val)
{
    dryGain->SetVal(val);
}

void Harp::onFrame(const Leap::Controller& controller)
{
    const Leap::HandList& hands = controller.frame().hands();
    if (hands.count() > 0)
    {
        float zAvg = 0.f;
        for (int i = 0; i < hands.count(); ++i)
        {
            const Leap::Hand& h = hands[i];
            zAvg += h.palmPosition().z;
        }
        zAvg /= (float) hands.count();
        float mix = 0.5f;
        if (zAvg < 0.f)
        {
            mix = -zAvg / 100.f;
        }
        else
            mix = zAvg / 250.f;
        if (mix > 1.f) mix = 1.f;
        if (mix < 0.f) mix = 0.f;
        setWetLevel(mix);
        setDryLevel(1-mix);
    }
    
}


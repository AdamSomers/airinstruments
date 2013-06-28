#include "MotionServer.h"

#include "Harp.h"


#define MAX_STRINGS 30
#define SAMPS_PER_PIXEL 6
#define FILTER_FREQ 20.f
#define FILTER_RES 0.f

const std::string* Harp::gScale = gPentatonicMajor;

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
, idle(true)
, active(false)
, chordMode(false)
, selectedScale(0)
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
        accumulators.push_back(new SampleAccumulator());
        accumulators.back()->SetInput(strings.back());
        accumulators.back()->SetSamplesPerPixel(SAMPS_PER_PIXEL);
    }
    
    mixer = new Adder;
    
    for (int i = 0; i < numStrings; ++i)
    {
        mixer->AddInput(accumulators.at(i));
    }
    
    filter = new StateVariable;
    filter->SetInput(mixer);
    filter->setFreq(80);
    filter->setType(StateVariable::kHighpass);

    preVerbFilter = new StateVariable;
    preVerbFilter->SetInput(filter);
    preVerbFilter->setFreq(1000.f);
    preVerbFilter->setType(StateVariable::kLowpass);
    
    reverbGain = new Multiplier;
    reverbGain->SetA(preVerbFilter);
    reverbGain->SetVal(wetLevel);
    
    reverb = new JuceReverbAudioClient;
    reverb->AddInput(reverbGain);
    
    dryGain = new Multiplier;
    dryGain->SetA(filter);
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
    
    for (int i = 0; i < 7; ++i)
        selectChord(i);
}

void Harp::Cleanup()
{
    for (int i = 0; i < strings.size(); ++i)
    {
        AudioServer::GetInstance()->EnterLock();
        AudioServer::GetInstance()->ExitLock();
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
    
    if (preVerbFilter)
        delete preVerbFilter;
    
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
}

void Harp::AddString()
{
    ScopedLock sl(lock);
    
    if (numStrings == MAX_STRINGS )
        return;
    
    strings.push_back(new Karplus(0.009));
    accumulators.push_back(new SampleAccumulator());
    accumulators.back()->SetInput(strings.back());
    accumulators.back()->SetSamplesPerPixel(SAMPS_PER_PIXEL);
    AudioServer::GetInstance()->EnterLock();
    mixer->AddInput(accumulators.back());
    AudioServer::GetInstance()->ExitLock();
    numStrings++;
    //outputGain->SetVal(fmax(1.f/numStrings, 1/12.f));
}

void Harp::RemoveString()
{
    ScopedLock sl(lock);
    
    if (numStrings == 1)
        return;
    AudioServer::GetInstance()->EnterLock();
    mixer->RemoveInput(accumulators.back());
    AudioServer::GetInstance()->ExitLock();

    delete accumulators.back();
    accumulators.pop_back();
    delete strings.back();
    strings.pop_back();
    numStrings--;
    //outputGain->SetVal(fmax(1.f/numStrings, 1/12.f));
}

void Harp::NoteOn(int num, int note, int velocity)
{
    ScopedLock sl(lock);
    
    strings.at(num)->NoteOn(note, velocity, NULL);
    idle = false;
}

void Harp::ExciteString(int num, int note, int velocity, float* buff, int bufferSize)
{
    ScopedLock sl(lock);
    
    strings.at(num)->NoteOn(note, velocity, buff, bufferSize);
    idle = false;
}

void Harp::SetScale(int scaleIndex)
{
    if (!chordMode)
    {
        selectedScale  = scaleIndex;
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

void Harp::selectChord(int chordIndex)
{
    auto iter = std::find(selectedChords.begin(), selectedChords.end(), chordIndex);
    if (iter == selectedChords.end())
        selectedChords.push_back(chordIndex);
    std::sort(selectedChords.begin(), selectedChords.end());
}

void Harp::deSelectChord(int chordIndex)
{
    auto iter = std::find(selectedChords.begin(), selectedChords.end(), chordIndex);
    if (iter != selectedChords.end())
        selectedChords.erase(iter);
}

bool Harp::isChordSelected(int chordIndex) const
{
    return std::find(selectedChords.begin(), selectedChords.end(), chordIndex) != selectedChords.end();
}

void Harp::setChordMode(bool shouldBeChordMode)
{
    chordMode = shouldBeChordMode;
    if (chordMode)
        SetScale(0);
    else
        SetScale(getSelectedScale());
}

int Harp::getNumSelectedChords() const
{
    return selectedChords.size();
}

void Harp::setChord(int chordIndex)
{
    //jassert(chordIndex < selectedChords.size());
    int chordNumber = *(selectedChords.begin() + chordIndex);
    SetScale(chordNumber);
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
        mix = jmax(0.5f, mix);
        setWetLevel(sqrt(mix));
        setDryLevel(sqrt(1-mix));
    }
    
}

void Harp::setActive(bool shouldBeActive)
{
    active = shouldBeActive;
    if (shouldBeActive)
    {
        AudioServer::GetInstance()->AddClient(outputGain, 0);
        AudioServer::GetInstance()->AddClient(outputGain, 1);
    }
    else
    {
        AudioServer::GetInstance()->RemoveClient(outputGain, 0);
        AudioServer::GetInstance()->RemoveClient(outputGain, 1);
    }
}

bool Harp::checkIdle()
{
    bool retVal = idle;
    idle = true;
    return retVal;
}

HarpManager::HarpManager()
{
    for (int i = 0; i < numHarps; ++i) {
        harps.push_back(new Harp);
        harps.back()->SetScale(i);
    }
}

HarpManager::~HarpManager()
{
    for (Harp* h : harps)
        delete h;
}

void HarpManager::activateHarp(int harpIndex)
{
    jassert(harpIndex < numHarps);
    Harp* h = harps.at(harpIndex);
    h->setActive(true);
}

void HarpManager::deActivateHarp(int harpIndex)
{
    jassert(harpIndex < numHarps);
    Harp* h = harps.at(harpIndex);
    h->setActive(false);
}

int HarpManager::getNumActiveHarps()
{
    int num = 0;
    for (Harp* h : harps)
    {
        if(h->isActive())
            ++num;
    }

    return num;
}

Harp* HarpManager::getHarp(int harpIndex)
{
    jassert(harpIndex < harps.size());
    return harps.at(harpIndex);
}


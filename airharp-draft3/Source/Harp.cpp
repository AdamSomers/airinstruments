#include "MotionServer.h"

#include "Harp.h"
#include "Main.h"

#define MAX_STRINGS 30
#define SAMPS_PER_PIXEL 6
#define FILTER_FREQ 20.f
#define FILTER_RES 0.f

Harp::Harp()
: numStrings(1)
, mixer(NULL)
, outputGain(NULL)
, reverb(NULL)
, filter(NULL)
, preVerbFilter(NULL)
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
    BuildDefaultScales();

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
    
    MotionDispatcher::instance().addListener(*this);
    
    for (int i = 0; i < 7; ++i) {
        bool chordSelected = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getBoolValue("chordSelected" + String(i), (i == 0 || i == 3 || i == 4 || i == 5) ? true : false);

        if (chordSelected)
            selectChord(i);
        
        // write the values back to initialize defaults
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("chordSelected" + String(i), chordSelected);
    }
    
    setChordMode(AirHarpApplication::getInstance()->getProperties().getUserSettings()->getBoolValue("chordMode", false));
    
    int selectedScale = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedScale", 0);
    SetScale(selectedScale);
}

void Harp::Cleanup()
{
    // Removing here causes a Juce memory leak.  Could be static deallocation order issue.
    //MotionDispatcher::instance().removeListener(*this);

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

void Harp::BuildDefaultScales()
{
    const std::string pentatonicMajor[] = { "1", "2", "4", "5", "6" };
    const std::string pentatonicMinor[] = {"1", "b3", "4", "5", "b7" };
    const std::string wholeTone[] = { "1", "2", "3", "#4", "#5", "#6" };
    const std::string diatonic[] = { "1", "2", "3", "4", "5", "6", "7"};
    const std::string minor[] = { "1", "2", "b3", "4", "5", "b6", "b7" };
    // Slendro?
    const std::string exotic1[] = { "1", "3", "4", "5", "7"};
    // Chinese mystery
    const std::string custom[] = { "1", "3", "#4", "5", "7" };
    
    const std::string I[]     = { "1", "3", "5" };
    const std::string ii[]    = { "2", "4", "6" };
    const std::string iii[]   = { "3", "5", "7" };
    const std::string IV[]    = { "1", "4", "6" };
    const std::string V[]     = { "2", "5", "7" };
    const std::string vi[]    = { "1", "3", "6" };
    const std::string VII[]   = { "2", "4", "b7" };
    const std::string vii_d[] = { "2", "4", "7" };
    
    const std::string i[]     = { "1", "b3", "5" };
    const std::string ii_d[]  = { "2", "4", "b6" };
    const std::string III[]   = { "b3", "5", "b7" };
    const std::string iv[]    = { "1", "4", "b6" };
    const std::string v[]     = { "2", "5", "b7" };
    const std::string VI[]    = { "1", "b3", "b6" };
    
    std::vector<std::string> scale;
    for (int i = 0; i <  5; ++i)
        scale.push_back(pentatonicMajor[i]);
    scales.insert(std::make_pair("pentatonicMajor", scale));
    scale.clear();

    for (int i = 0; i <  5; ++i)
        scale.push_back(pentatonicMinor[i]);
    scales.insert(std::make_pair("pentatonicMinor", scale));
    scale.clear();
    
    for (int i = 0; i <  6; ++i)
        scale.push_back(wholeTone[i]);
    scales.insert(std::make_pair("wholeTone", scale));
    scale.clear();
    
    for (int i = 0; i <  7; ++i)
        scale.push_back(diatonic[i]);
    scales.insert(std::make_pair("diatonic", scale));
    scale.clear();
    
    for (int i = 0; i <  7; ++i)
        scale.push_back(minor[i]);
    scales.insert(std::make_pair("minor", scale));
    scale.clear();
    
    for (int i = 0; i <  5; ++i)
        scale.push_back(exotic1[i]);
    scales.insert(std::make_pair("exotic1", scale));
    scale.clear();
    
    for (int i = 0; i <  5; ++i)
        scale.push_back(custom[i]);
    scales.insert(std::make_pair("custom", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(I[i]);
    scales.insert(std::make_pair("I", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(ii[i]);
    scales.insert(std::make_pair("ii", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(iii[i]);
    scales.insert(std::make_pair("iii", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(IV[i]);
    scales.insert(std::make_pair("IV", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(V[i]);
    scales.insert(std::make_pair("V", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(vi[i]);
    scales.insert(std::make_pair("vi", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(VII[i]);
    scales.insert(std::make_pair("VII", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(vii_d[i]);
    scales.insert(std::make_pair("vii_d", scale));
    scale.clear();
    
    for (int j = 0; j < 3; ++j)
        scale.push_back(i[j]);
    scales.insert(std::make_pair("i", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(ii_d[i]);
    scales.insert(std::make_pair("ii_d", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(III[i]);
    scales.insert(std::make_pair("III", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(iv[i]);
    scales.insert(std::make_pair("iv", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(v[i]);
    scales.insert(std::make_pair("v", scale));
    scale.clear();
    
    for (int i = 0; i < 3; ++i)
        scale.push_back(VI[i]);
    scales.insert(std::make_pair("VI", scale));
    scale.clear();
    
    String customScale = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("customScale", "1 M3 b5 P5 M7");
    StringArray arr;
    arr.addTokens(customScale, false);
    std::vector<std::string> vec;
    for (int i = 0; i < arr.size(); ++i)
    {
        vec.push_back(arr[i].toStdString());
    }
    setCustomScale(vec);

}

std::vector<std::string>& Harp::getScale()
{
    ScaleMap::iterator i = scales.find(selectedScaleName);
    jassert(i != scales.end());
    return (*i).second;
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
                selectedScaleName = "diatonic";
                break;
            case 1:
                selectedScaleName = "minor";
                break;
            case 2:
                selectedScaleName = "pentatonicMajor";
                break;
            case 3:
                selectedScaleName = "pentatonicMinor";
                break;
            case 4:
                selectedScaleName = "wholeTone";
                break;
            case 5:
                selectedScaleName = "exotic1";
                break;
            case 6:
                selectedScaleName = "custom";
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
                selectedScaleName = minor ? "i" : "I";
                break;
            case 1:
                selectedScaleName = minor ? "ii_d" : "ii";
                break;
            case 2:
                selectedScaleName = minor ? "III" : "iii";
                break;
            case 3:
                selectedScaleName = minor ? "iv" : "IV";
                break;
            case 4:
                selectedScaleName = minor ? "v" : "V";
                break;
            case 5:
                selectedScaleName = minor ? "VI" : "vi";
                break;
            case 6:
                selectedScaleName = minor ? "VII" : "vii_d";
                break;
            default:
                break;
        }
    }
}

void Harp::setCustomScale(std::vector<std::string>& newCustomScale)
{
    ScaleMap::iterator i = scales.find("custom");
    if (i != scales.end())
    {
        (*i).second = newCustomScale;
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
        //harps.back()->SetScale(i);
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


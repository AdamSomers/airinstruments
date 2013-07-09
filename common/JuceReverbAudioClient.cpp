#include "JuceReverbAudioClient.h"

JuceReverbAudioClient::JuceReverbAudioClient()
{
    Reverb::Parameters params;
    params.roomSize = .8f;
    params.wetLevel = .5f;
    params.dryLevel = 0.f;
    params.damping = 1.f;
    params.width = 1.f;
    params.freezeMode = 0.f;
    reverb.setParameters(params);
}

JuceReverbAudioClient::~JuceReverbAudioClient()
{
    
}

void JuceReverbAudioClient::Render(float* buffer, const int frames)
{
    float* tmp = new float[frames];
    memset(tmp, 0.f, frames * sizeof(float));
    
    std::vector<AudioClient*>::iterator i;
    for (i = fClients.begin(); i != fClients.end(); ++i)
    {
        (*i)->Process(buffer, frames);
        for (int i = 0; i < frames; ++i)
        {
            tmp[i] += buffer[i];
        }
    }
    
    reverb.processMono(tmp, frames);
    
    for (int i = 0; i < frames; ++i)
    {
        buffer[i] = tmp[i];
    }

    delete[] tmp;
}

void JuceReverbAudioClient::AddInput(AudioClient* c)
{
    std::vector<AudioClient*>::iterator i = std::find(fClients.begin(), fClients.end(), c);
    if (i == fClients.end())
    {
        fClients.push_back(c);
    }
}

void JuceReverbAudioClient::RemoveInput(AudioClient* c)
{
    std::vector<AudioClient*>::iterator i = std::find(fClients.begin(), fClients.end(), c);
    if (i != fClients.end())
    {
        fClients.erase(i);
    }
}
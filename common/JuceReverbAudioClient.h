
#ifndef h_JuceReverbAudioClient
#define h_JuceReverbAudioClient

#include <iostream>
#include "AudioClient.h"

#include "../JuceLibraryCode/JuceHeader.h"

class JuceReverbAudioClient : public AudioClient
{
public:
    JuceReverbAudioClient();
    ~JuceReverbAudioClient();
	// AudioClient override
	void Render(float* buffer, int frames);
    
    void AddInput(AudioClient* c);
	void RemoveInput(AudioClient* c);
    
    Reverb reverb;
private:
    std::vector<AudioClient*> fClients;
};

#endif // h_JuceReverbAudioClient

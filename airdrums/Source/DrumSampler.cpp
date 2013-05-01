/*
  ==============================================================================

    DrumSampler.cpp
    Created: 30 Apr 2013 3:41:59pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumSampler.h"
#include "LayeredSamplerSound.h"


DrumSampler::DrumSampler()
{
}


DrumSampler::~DrumSampler()
{
}


void DrumSampler::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    const ScopedLock sl (lock);

    for (int i = sounds.size(); --i >= 0;)
    {
        SynthesiserSound* const sound = sounds.getUnchecked(i);

        if (sound->appliesToNote (midiNoteNumber)
             && sound->appliesToChannel (midiChannel))
        {
			LayeredSamplerSound* layer = dynamic_cast<LayeredSamplerSound*> (sound);
			if (layer != nullptr)
			{
				if (!layer->AppliesToVelocity((int) (velocity * 127)))
					continue;
			}

            // If hitting a note that's still ringing, stop it first (it could be
            // still playing because of the sustain or sostenuto pedal).
            for (int j = voices.size(); --j >= 0;)
            {
                SynthesiserVoice* const voice = voices.getUnchecked (j);

                if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                     && voice->isPlayingChannel (midiChannel))
					voice->stopNote (true);
            }

            startVoice (findFreeVoice (sound, isNoteStealingEnabled()),
                        sound, midiChannel, midiNoteNumber, velocity);
        }
    }
}

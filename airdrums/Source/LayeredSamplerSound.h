/*
  ==============================================================================

    LayeredSamplerSound.h
    Created: 30 Apr 2013 3:44:33pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __LAYEREDSAMPLERSOUND_H_3F001D56__
#define __LAYEREDSAMPLERSOUND_H_3F001D56__

#include "../JuceLibraryCode/JuceHeader.h"


class LayeredSamplerSound : public SamplerSound
{
public:
	LayeredSamplerSound(const String& name, AudioFormatReader& source, const BigInteger& midiNotes, int midiNoteForNormalPitch, double attackTimeSecs,
						double releaseTimeSecs, double maxSampleLengthSeconds, int minVelocity = 0, int maxVelocity = 127);
	virtual ~LayeredSamplerSound();

	bool	AppliesToVelocity(int velocity);

private:
	int		mMinVelocity;
	int		mMaxVelocity;
};

#endif  // __LAYEREDSAMPLERSOUND_H_3F001D56__

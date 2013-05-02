/*
  ==============================================================================

    LayeredSamplerSound.cpp
    Created: 30 Apr 2013 3:44:16pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "LayeredSamplerSound.h"


LayeredSamplerSound::LayeredSamplerSound(const String& name, AudioFormatReader& source, const BigInteger& midiNotes, int midiNoteForNormalPitch, double attackTimeSecs,
					double releaseTimeSecs, double maxSampleLengthSeconds, int minVelocity /*= 0*/, int maxVelocity /*= 127*/) :
	SamplerSound(name, source, midiNotes, midiNoteForNormalPitch, attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds),
	mMinVelocity(minVelocity),
	mMaxVelocity(maxVelocity)
{
}


LayeredSamplerSound::~LayeredSamplerSound()
{
}


bool LayeredSamplerSound::AppliesToVelocity(int velocity)
{
	return (velocity >= mMinVelocity) && (velocity <= mMaxVelocity);
}

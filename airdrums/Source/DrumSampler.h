/*
  ==============================================================================

    DrumSampler.h
    Created: 30 Apr 2013 3:42:10pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMSAMPLER_H_71D57161__
#define __DRUMSAMPLER_H_71D57161__

#include "../JuceLibraryCode/JuceHeader.h"


class DrumSampler : public Synthesiser
{
public:
	DrumSampler();
	virtual ~DrumSampler();

	virtual void noteOn(int midiChannel, int midiNoteNumber, float velocity);

private:
};


#endif  // __DRUMSAMPLER_H_71D57161__

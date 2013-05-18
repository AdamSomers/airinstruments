/*
  ==============================================================================

    SampleLayer.h
    Created: 30 Apr 2013 3:40:52pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __SAMPLELAYER_H_4E11FC66__
#define __SAMPLELAYER_H_4E11FC66__

#include "../JuceLibraryCode/JuceHeader.h"

#include "LayeredSamplerSound.h"


class SampleLayer
{
public:
	enum Status
	{
		kNoError = 0,
		kFilenameError = 1,
		kFileNotFoundError = 2,
		kFileLoadError = 3,
		kVelocityError = 4,
		kNoteNumberError = 5
	};

	SampleLayer();
	~SampleLayer();

	Status	LoadFromXml(int note, XmlElement* element, File& directory);
	Status	LoadFromFile(int note, String filename, File& directory, int minVelocity = 0, int maxVelocity = 127);
	Status	CreateFromMemory(const char* data, int size, int note);
	SynthesiserSound::Ptr	GetSound(void);

private:
	SynthesiserSound::Ptr	mSound;
};

#endif  // __SAMPLELAYER_H_4E11FC66__

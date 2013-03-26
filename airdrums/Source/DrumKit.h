/*
  ==============================================================================

    DrumKit.h
    Created: 25 Mar 2013 1:39:50pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMKIT_H_FC36DC2F__
#define __DRUMKIT_H_FC36DC2F__

#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

#include "Types.h"
#include "DrumSample.h"


class DrumKit
{
public:
	enum Status
	{
		kNoError = 0,
		kSampleLoadError = 1,
		kNoSamplesError = 2
	};

	DrumKit();
	~DrumKit();

	// Initialization

	Status	LoadFromXml(XmlElement* element, File& directory);

	// Access to the kit's samples for playback

	SharedPtr<DrumSample>	GetSample(int index);

	// Other accessors

	int	GetSampleCount(void);

	String&	GetName(void);
	//Image	GetImage(void);

private:
	String	mName;
	//Image	mImage;

	typedef	SharedPtr<DrumSample>	Item;
	typedef	std::vector<Item>		Container;
	typedef	Container::iterator		Iterator;

	Container	mSamples;
};

#endif  // __DRUMKIT_H_FC36DC2F__

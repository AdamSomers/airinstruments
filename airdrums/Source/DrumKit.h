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
#include "DrumItem.h"


class DrumKit : public DrumItem
{
public:
	DrumKit();
	~DrumKit();

	// Initialization

	Status	LoadFromXml(XmlElement* element, File& directory);
	Status	SaveToXml(String fileName, File& directory);

	// Access to the kit's samples for playback

	SharedPtr<DrumSample>	GetSample(int index);

	// Other accessors

	int	GetSampleCount(void);

    
    void LoadTextures();
    GLuint GetTexture() const;
    const Image& GetImage() const;

private:
	typedef	SharedPtr<DrumSample>	Item;
	typedef	std::vector<Item>		Container;
	typedef	Container::iterator		Iterator;

	Container	mSamples;
    
    Image	mImage;
    GLuint  mTextureId;
};

#endif  // __DRUMKIT_H_FC36DC2F__

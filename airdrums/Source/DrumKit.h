/*
  ==============================================================================

    DrumKit.h
    Created: 25 Mar 2013 1:39:50pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMKIT_H_FC36DC2F__
#define __DRUMKIT_H_FC36DC2F__

#include <vector>

#include "GfxTools.h"
#include "Types.h"
#include "DrumSample.h"
#include "DrumItem.h"

#include "../JuceLibraryCode/JuceHeader.h"


class DrumKit : public DrumItem<DrumKit>
{
public:
	DrumKit();
	~DrumKit();

	// Initialization

	Status	LoadFromXml(XmlElement* element, File& directory);
	Status	SaveToXml(String fileName, File& directory);

	// Access to the kit's samples for playback

	SharedPtr<DrumSample>	GetSample(int index);
	SharedPtr<DrumSample>	GetMetronome(void);

	// Other accessors

	int	GetSampleCount(void);
    
    void LoadTextures();
    void SetTexture(TextureDescription texture);
    TextureDescription GetTexture() const;
    const Image& GetImage() const;

	struct MemorySampleInfo		// Used to describe a memory based audio sample
	{
		const char*	data;		// Pointer to the sample data (aiff audio file format in binary form)
		int			size;		// Size of the data in bytes
		int			note;		// MIDI note number of this sample in the drum kit
		const char*	category;	// Category name of this sample in the drum kit
	};

	Status	CreateFromMemory(std::vector<MemorySampleInfo>& samples, String name, Uuid uuid);

private:
	typedef	SharedPtr<DrumSample>	Item;
	typedef	std::vector<Item>		Container;
	typedef	Container::iterator		Iterator;

	Container	mSamples;
	Item		mMetronome;
    
    Image	mImage;
    TextureDescription  mTextureDesc;
};

#endif  // __DRUMKIT_H_FC36DC2F__

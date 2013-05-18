/*
  ==============================================================================

    DrumSample.h
    Created: 25 Mar 2013 1:40:21pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMSAMPLE_H_3AC03E5__
#define __DRUMSAMPLE_H_3AC03E5__

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include "Types.h"
#include "SampleLayer.h"
#include "GfxTools.h"

class DrumSample
{
public:
	enum Status
	{
		kNoError = 0,
		kNoteNumberError = 1,
		kCategoryError = 2,
		kLayerError = 3,
		kNoLayersError = 4
	};

	DrumSample();
	~DrumSample();

	// Initialization

	Status	LoadFromXml(XmlElement* element, File& directory);
	Status	CreateFromMemory(const char* data, int size, int note, String category);
    
    // This must only be called from the OpenGL thread
    void LoadTextures();

	// Other accessors

	int		GetNoteNumber(void);
	String&	GetCategory(void);
	//Image	GetImage(void);
    TextureDescription	GetTexture(bool on) const;
	SynthesiserSound::Ptr	GetSound(int layer);
	int		GetLayerCount(void);

private:
	typedef	SharedPtr<SampleLayer>	Item;
	typedef	std::vector<Item>		Container;
	typedef	Container::iterator		Iterator;

	int			mNoteNumber;
	String		mCategory;
	Image		mImage;
    TextureDescription		mTextureDesc;
	Container	mLayers;
};

#endif  // __DRUMSAMPLE_H_3AC03E5__

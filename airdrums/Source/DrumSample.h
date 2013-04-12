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

#include "Types.h"


class DrumSample
{
public:
	enum Status
	{
		kNoError = 0,
		kNoteNumberError = 1,
		kCategoryError = 2,
		kFilenameError = 3,
		kFileNotFoundError = 4,
		kFileLoadError = 5
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
    GLuint GetTexture() const;
	SynthesiserSound::Ptr	GetSound(void);

private:
	int		mNoteNumber;
	String	mCategory;
	Image	mImage;
    GLuint  mTextureId;

	SynthesiserSound::Ptr	mSound;
};

#endif  // __DRUMSAMPLE_H_3AC03E5__

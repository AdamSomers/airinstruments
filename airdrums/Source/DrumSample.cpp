/*
  ==============================================================================

    DrumSample.cpp
    Created: 25 Mar 2013 1:40:21pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "GfxTools.h"
#include "SkinManager.h"
#include "DrumSample.h"

DrumSample::DrumSample()
: mTextureId(0)
{
}


DrumSample::~DrumSample()
{
}


DrumSample::Status DrumSample::LoadFromXml(XmlElement* element, File& directory)
{
	int note = element->getIntAttribute("note", -1);
	if ((note < 0) || (note > 127))
		return kNoteNumberError;
	String category = element->getStringAttribute("category", "");
	if (category == "")
		return kCategoryError;
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }
	String filename = element->getStringAttribute("file", "");
	if (filename == "")
		return kFilenameError;
	File file = directory.getChildFile(filename);
	if (!file.existsAsFile()) {
        Logger::outputDebugString("Sample file " + file.getFileName() + " not found");
		return kFileNotFoundError;
    }

	FileInputStream* stream(new FileInputStream(file));
    AiffAudioFormat aiffFormat;
	ScopedPointer<AudioFormatReader> reader(aiffFormat.createReaderFor(stream, true));
    BigInteger notes;
    notes.setRange(note, 1, true);
	SamplerSound* sound = new SamplerSound("", *reader, notes, note, 0.0, 0.1, 10.0);
	if (sound->getAudioData() == nullptr)
		return kFileLoadError;

	mNoteNumber = note;
	mCategory = category;
	mSound = sound;

	return kNoError;
}


DrumSample::Status DrumSample::CreateFromMemory(const char* data, int size, int note, String category)
{
	if ((note < 0) || (note > 127))
		return kNoteNumberError;
	if (category == "")
		return kCategoryError;

/*
	// TODO :  The built-in kit needs images ?  If so, the image data needs to be made binary and passed in to this method.
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }
*/

	MemoryInputStream* stream(new MemoryInputStream(data, size, false));
    AiffAudioFormat aiffFormat;
	ScopedPointer<AudioFormatReader> reader(aiffFormat.createReaderFor(stream, true));
    BigInteger notes;
    notes.setRange(note, 1, true);
	SamplerSound* sound = new SamplerSound("", *reader, notes, note, 0.0, 0.1, 10.0);

	mNoteNumber = note;
	mCategory = category;
	mSound = sound;

	return kNoError;
}


SynthesiserSound::Ptr DrumSample::GetSound(void)
{
	return mSound;
}


int DrumSample::GetNoteNumber(void)
{
	return mNoteNumber;
}


String& DrumSample::GetCategory(void)
{
	return mCategory;
}

GLuint DrumSample::GetTexture(bool on) const
{
    if (mImage.isValid())
        return mTextureId;
    else {
        String imageName = mCategory;
        if (on)
            imageName += "_on";
        return SkinManager::instance().getSelectedSkin().getTexture(imageName);
    }
}

void DrumSample::LoadTextures()
{
    if (mImage.isValid())
    {
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        GfxTools::loadTextureFromJuceImage(mImage);
    }
}

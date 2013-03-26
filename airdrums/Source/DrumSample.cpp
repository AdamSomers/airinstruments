/*
  ==============================================================================

    DrumSample.cpp
    Created: 25 Mar 2013 1:40:21pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumSample.h"


DrumSample::DrumSample()
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
	String filename = element->getStringAttribute("file", "");
	if (filename == "")
		return kFilenameError;
	File file = directory.getChildFile(filename);
	if (!file.existsAsFile())
		return kFileNotFoundError;

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

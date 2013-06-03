/*
  ==============================================================================

    SampleLayer.cpp
    Created: 30 Apr 2013 3:40:33pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "SampleLayer.h"


SampleLayer::SampleLayer()
{
}


SampleLayer::~SampleLayer()
{
}


SampleLayer::Status SampleLayer::LoadFromXml(int note, XmlElement* element, File& directory)
{
	int minVelocity = element->getIntAttribute("minVelocity", -1);
	if ((minVelocity < 0) || (minVelocity > 127))
		return kVelocityError;
	int maxVelocity = element->getIntAttribute("maxVelocity", -1);
	if ((maxVelocity < 0) || (maxVelocity > 127) || (minVelocity > maxVelocity))
		return kVelocityError;

	String filename = element->getStringAttribute("file", "");

	return LoadFromFile(note, filename, directory, minVelocity, maxVelocity);
}


SampleLayer::Status SampleLayer::LoadFromFile(int note, String filename, File& directory, int minVelocity /*= 0*/, int maxVelocity /*= 127*/)
{
	if (filename == "")
		return kFilenameError;
	File file = directory.getChildFile(filename);
	if (!file.existsAsFile()) {
        Logger::writeToLog("Sample file " + file.getFileName() + " not found");
		return kFileNotFoundError;
    }

	FileInputStream* stream(new FileInputStream(file));
    AiffAudioFormat aiffFormat;
	ScopedPointer<AudioFormatReader> reader(aiffFormat.createReaderFor(stream, true));
    BigInteger notes;
    notes.setRange(note, 1, true);
	SamplerSound* sound = new LayeredSamplerSound("", *reader, notes, note, 0.0, 0.1, 10.0, minVelocity, maxVelocity);
	if (sound->getAudioData() == nullptr)
		return kFileLoadError;

	mSound = sound;

	return kNoError;
}


SampleLayer::Status SampleLayer::CreateFromMemory(const char* data, int size, int note)
{
	if ((note < 0) || (note > 127))
		return kNoteNumberError;

	MemoryInputStream* stream(new MemoryInputStream(data, size, false));
    AiffAudioFormat aiffFormat;
	ScopedPointer<AudioFormatReader> reader(aiffFormat.createReaderFor(stream, true));
    BigInteger notes;
    notes.setRange(note, 1, true);
	SamplerSound* sound = new LayeredSamplerSound("", *reader, notes, note, 0.0, 0.1, 10.0);

	mSound = sound;

	return kNoError;
}


SynthesiserSound::Ptr SampleLayer::GetSound(void)
{
	return mSound;
}

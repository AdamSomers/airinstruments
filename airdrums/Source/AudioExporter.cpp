/*
  ==============================================================================

    AudioExporter.cpp
    Created: 8 May 2013 5:03:08pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "Main.h"
#include "AudioExporter.h"
#include "Drums.h"

#include "../JuceLibraryCode/JuceHeader.h"


AudioExporter::AudioExporter()
{
}


AudioExporter::~AudioExporter()
{
}


void AudioExporter::Export(void)
{
	AirHarpApplication* app = AirHarpApplication::getInstance();
	Drums& drums = Drums::instance();
	Drums::TransportState& transport = drums.getTransportState();

	transport.pause();

	File file("Export.wav");
	FileOutputStream* stream = new FileOutputStream(file);
	WavAudioFormat format;
	UniquePtr<AudioFormatWriter> writer(format.createWriterFor(stream, drums.getSampleRate(), 2, 32, StringPairArray(), 0));
	jassert(writer.get() != nullptr);
	const long blockSize = 2048;

	drums.AllNotesOff();
	app->StopAudioDevice();
	drums.resetToZero();

	long totalSamples = drums.getMaxSamples();
	long samplesToGo = totalSamples;
	transport.doExport();
	transport.play();
	while (samplesToGo > 0)
	{
		long count = jmin(samplesToGo, blockSize);
		samplesToGo -= count;
		bool status = writer->writeFromAudioSource(drums, count, count);
		if (!status)
		{
			// May need some error dialog here
			jassertfalse;
			break;
		}
	}
	transport.pause();
	drums.AllNotesOff();

	app->StartAudioDevice();
}

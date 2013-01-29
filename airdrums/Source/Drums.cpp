#include "Drums.h"

Drums::Drums()
{
    for (int i = 0; i < 16; ++i)
        synth.addVoice (new SamplerVoice());
    WavAudioFormat wavFormat;
    AiffAudioFormat aiffFormat;
    
    ScopedPointer<AudioFormatReader> audioReader8 (wavFormat.createReaderFor (new MemoryInputStream (BinaryData::JAZ_Rhabdedlumrlr_wav,
                                                                                                    BinaryData::JAZ_Rhabdedlumrlr_wavSize,
                                                                                                    false),
                                                                             true));
    ScopedPointer<AudioFormatReader> audioReader15 (wavFormat.createReaderFor (new MemoryInputStream (BinaryData::MeanBeat_HH_Closed_Pop_wav,
                                                                                                     BinaryData::MeanBeat_HH_Closed_Pop_wavSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> audioReader14 (wavFormat.createReaderFor (new MemoryInputStream (BinaryData::MeanBeat_HH_Open_Pure_wav,
                                                                                                     BinaryData::MeanBeat_HH_Open_Pure_wavSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> audioReader12 (wavFormat.createReaderFor (new MemoryInputStream (BinaryData::FD27_DRM_WAV,
                                                                                                     BinaryData::FD27_DRM_WAVSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> audioReader13 (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::_15GKIT02E_aif,
                                                                                                     BinaryData::_15GKIT02E_aifSize,
                                                                                                     false),
                                                                              true));
    BigInteger notes;
    notes.setRange (8, 1, true);
    synth.addSound (new SamplerSound ("", *audioReader8, notes, 8, 0.1, 0.1, 10.0));
    notes.setRange (12, 1, true);
    synth.addSound (new SamplerSound ("", *audioReader12, notes, 12, 0.1, 0.1, 10.0));
    notes.setRange (13, 1, true);
    synth.addSound (new SamplerSound ("", *audioReader13, notes, 13, 0.1, 0.1, 10.0));
    notes.setRange (14, 1, true);
    synth.addSound (new SamplerSound ("", *audioReader14, notes, 14, 0.1, 0.1, 10.0));
    notes.setRange (15, 1, true);
    synth.addSound (new SamplerSound ("", *audioReader15, notes, 15, 0.1, 0.1, 10.0));
    synth.setNoteStealingEnabled(false);
}

Drums::~Drums()
{
}

void Drums::NoteOn(int note, float velocity)
{
    keyboardState.noteOn(1,note,velocity);
}

void Drums::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    midiCollector.reset (sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void Drums::releaseResources()
{
    
}

void Drums::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
    keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
}

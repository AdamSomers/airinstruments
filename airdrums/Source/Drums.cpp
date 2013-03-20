#include "Drums.h"

Drums::Drums() :
    recording(false),
    metronomeOn(true),
    sampleCounter(0),
    maxRecordSamples(0),
    tempo(118),
    numNotes(0),
    sampleRate(44100)
{
   
    for (int i = 0; i < 16; ++i)
        synth.addVoice (new SamplerVoice());
    WavAudioFormat wavFormat;
    AiffAudioFormat aiffFormat;
    
    ScopedPointer<AudioFormatReader> deepKick (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_BKICK_aif,
                                                                                                    BinaryData::TMD_CHIL_BKICK_aifSize,
                                                                                                    false),
                                                                             true));
    ScopedPointer<AudioFormatReader> chh (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_CHH_aif,
                                                                                                     BinaryData::TMD_CHIL_CHH_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> clap (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_CLAP_aif,
                                                                                                     BinaryData::TMD_CHIL_CLAP_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> clv (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_CLV_aif,
                                                                                                     BinaryData::TMD_CHIL_CLV_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> cym (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_CYM_aif,
                                                                                                     BinaryData::TMD_CHIL_CYM_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> htom (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_HTOM_aif,
                                                                                                     BinaryData::TMD_CHIL_HTOM_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> kick (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_KICK_aif,
                                                                                                     BinaryData::TMD_CHIL_KICK_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> ltom (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_LTOM_aif,
                                                                                                     BinaryData::TMD_CHIL_LTOM_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> mtom (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_MTOM_aif,
                                                                                                     BinaryData::TMD_CHIL_MTOM_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> ohh (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_OHH_aif,
                                                                                                     BinaryData::TMD_CHIL_OHH_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> pad (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_PAD_aif,
                                                                                                     BinaryData::TMD_CHIL_PAD_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> phh (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_PHH_aif,
                                                                                                     BinaryData::TMD_CHIL_PHH_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> ride (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_RIDE_aif,
                                                                                                     BinaryData::TMD_CHIL_RIDE_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> sfx (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_SFX_aif,
                                                                                                     BinaryData::TMD_CHIL_SFX_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> sn (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_SN_1_aif,
                                                                                                     BinaryData::TMD_CHIL_SN_1_aifSize,
                                                                                                     false),
                                                                              true));
    ScopedPointer<AudioFormatReader> verb (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_VERB_aif,
                                                                                                     BinaryData::TMD_CHIL_VERB_aifSize,
                                                                                                     false),
                                                                              true));
    BigInteger notes;
    notes.setRange (0, 1, true);
    synth.addSound (new SamplerSound ("", *pad, notes, 0, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (1, 1, true);
    synth.addSound (new SamplerSound ("", *sfx, notes, 1, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (2, 1, true);
    synth.addSound (new SamplerSound ("", *verb, notes, 2, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (3, 1, true);
    synth.addSound (new SamplerSound ("", *deepKick, notes, 3, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (4, 1, true);
    synth.addSound (new SamplerSound ("", *ltom, notes, 4, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (5, 1, true);
    synth.addSound (new SamplerSound ("", *mtom, notes, 5, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (6, 1, true);
    synth.addSound (new SamplerSound ("", *htom, notes, 6, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (7, 1, true);
    synth.addSound (new SamplerSound ("", *clv, notes, 7, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (8, 1, true);
    synth.addSound (new SamplerSound ("", *cym, notes, 8, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (9, 1, true);
    synth.addSound (new SamplerSound ("", *clap, notes, 9, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (10, 1, true);
    synth.addSound (new SamplerSound ("", *ohh, notes, 10, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (11, 1, true);
    synth.addSound (new SamplerSound ("", *ride, notes, 11, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (12, 1, true);
    synth.addSound (new SamplerSound ("", *kick, notes, 12, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (13, 1, true);
    synth.addSound (new SamplerSound ("", *sn, notes, 13, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (14, 1, true);
    synth.addSound (new SamplerSound ("", *phh, notes, 14, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (15, 1, true);
    synth.addSound (new SamplerSound ("", *chh, notes, 15, 0.0, 0.1, 10.0));
    notes.clear();
    notes.setRange (16, 1, true);
    synth.addSound (new SamplerSound ("", *clv, notes, 16, 0.0, 0.1, 10.0));

    numNotes = 16;
    
    synth.setNoteStealingEnabled(false);
}

Drums::~Drums()
{
}

void Drums::NoteOn(int note, float velocity)
{
    midiBufferLock.enter();
    if (recording) {
        float bps = tempo / 60.f;
        float sixteenthsPerSecond = bps * 4;
        int samplesPerSixteenth = (int) (sampleRate / sixteenthsPerSecond);
        float sixteenthsIntoPattern = sampleCounter / (float)samplesPerSixteenth;
        int quantizedPosition = (int)sixteenthsIntoPattern * samplesPerSixteenth;
        float diff = sixteenthsIntoPattern - (int)sixteenthsIntoPattern;
        Logger::outputDebugString(String::formatted("%f\n", diff));
        if (diff > 0.5) {
            quantizedPosition += samplesPerSixteenth;
            if (quantizedPosition > maxRecordSamples)
                quantizedPosition = 0;
        }
        
        MidiMessage m = MidiMessage::noteOn(1, note, velocity);
        m.setTimeStamp(quantizedPosition);
        MidiBuffer::Iterator i(recordBuffer);
        i.setNextSamplePosition(quantizedPosition);
        int position;
        MidiMessage existingMessage;
        bool found = i.getNextEvent(existingMessage, position);
        if (!found || position != quantizedPosition || existingMessage.getNoteNumber() != note)
            recordBuffer.addEvent(m, quantizedPosition);
        
        if (diff < 0.5) {
            keyboardState.noteOn(1,note,velocity);
            playbackState.noteOn(1,note,velocity);
        }
    }
    else {
        keyboardState.noteOn(1,note,velocity);
        playbackState.noteOn(1,note,velocity);
    }
    midiBufferLock.exit();
}

void Drums::clear()
{
    midiBufferLock.enter();
    recordBuffer.clear();
    midiBufferLock.exit();
}

void Drums::clearTrack(int note)
{
    midiBufferLock.enter();
    MidiBuffer::Iterator i(recordBuffer);
    int samplePos = 0;
    MidiMessage message;
    MidiBuffer newBuffer;
    while (i.getNextEvent(message, samplePos))
    {
        if (message.getNoteNumber() != note)
            newBuffer.addEvent(message, samplePos);
    }
    recordBuffer = newBuffer;
    midiBufferLock.exit();
}

void Drums::prepareToPlay (int /*samplesPerBlockExpected*/, double inSampleRate)
{
    sampleRate = inSampleRate;
    midiCollector.reset (sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    float bps = tempo / 60.f;
    int numBeats = 8;
    float seconds = numBeats / bps;
    float samples = sampleRate * seconds;
    long oldMaxRecordSample = maxRecordSamples;
    maxRecordSamples = (long) samples;
    long metronomePos = 0;
    
    // Adjust the metronome buffer
    metronomeBuffer.clear();
    for (int i = 0; i < numBeats; ++i) {
        metronomeBuffer.addEvent(MidiMessage::noteOn(1, 16, 1.f), metronomePos);
        metronomePos += (long)(samples / numBeats);
    }
    
    // Adjust sample positions of all events in record buffer 
    midiBufferLock.enter();
    MidiBuffer::Iterator i(recordBuffer);
    int samplePos = 0;
    MidiMessage message;
    MidiBuffer newBuffer;
    while (i.getNextEvent(message, samplePos))
    {
        double positionRatio = samplePos / (double)oldMaxRecordSample;
        long newPos = (long)(positionRatio * (double)maxRecordSamples);
        newBuffer.addEvent(message, newPos);
    }
    recordBuffer = newBuffer;
    midiBufferLock.exit();
    
    
}

void Drums::releaseResources()
{
    
}

void Drums::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    midiBufferLock.enter();
    bufferToFill.clearActiveBufferRegion();
    
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
    
    if (recording) {
        playbackState.reset();
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
        MidiBuffer::Iterator i(recordBuffer);
        int samplePos = sampleCounter;
        MidiMessage message;
        i.setNextSamplePosition(sampleCounter);
        while (i.getNextEvent(message, samplePos) && samplePos < sampleCounter + bufferToFill.numSamples)
        {
            //Logger::outputDebugString(String::formatted("%d\n", samplePos));
            incomingMidi.addEvent(message, samplePos - sampleCounter);
            playbackState.noteOn(1, message.getNoteNumber(),1);
        }
        
        if (metronomeOn) {
            MidiBuffer::Iterator metronomeIterator(metronomeBuffer);
            metronomeIterator.setNextSamplePosition(sampleCounter);
            while (metronomeIterator.getNextEvent(message, samplePos) && samplePos < sampleCounter + bufferToFill.numSamples)
            {
                //Logger::outputDebugString(String::formatted("%d\n", samplePos));
                incomingMidi.addEvent(message, 0);
            }
        }
        
    }
    else
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
    
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    
    sampleCounter += bufferToFill.numSamples;
    if (sampleCounter > maxRecordSamples)
        sampleCounter = 0;
    midiBufferLock.exit();
}

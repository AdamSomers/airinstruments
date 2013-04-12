#include "Main.h"
#include "Drums.h"
#include "KitManager.h"
#include "PatternManager.h"
#include "DrumPattern.h"


Drums::Drums() :
    transportState(false,false,true),
    sampleCounter(0),
    maxRecordSamples(0),
    numNotes(0),
    sampleRate((double) DrumPattern::kDefaultSampleRate),
	tempoSlider(nullptr)
{
	AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	globalTempo = (float) props.getUserSettings()->getDoubleValue("tempo", (double) DrumPattern::kDefaultTempo);

	if (props.getUserSettings()->getBoolValue("tempoSource", kGlobalTempo) != kGlobalTempo)
		tempoSource = kPatternTempo;
	else
		tempoSource = kGlobalTempo;

    for (int i = 0; i < 16; ++i)
        synth.addVoice (new SamplerVoice());
    
	KitManager& kmgr = KitManager::GetInstance();
	KitManager::Status kstatus = kmgr.BuildKitList();
	int count = kmgr.GetItemCount();
	if ((kstatus == KitManager::kNoError) && (count > 0))
	{
		setDrumKit(kmgr.GetItem(0));	// Use first kit for now
	}
	else
	{
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Initialization error", "There was an error loading drum kits");
        app->quit();
        return;
	}

    synth.setNoteStealingEnabled(false);
}

Drums::~Drums()
{
}

void Drums::NoteOn(int note, float velocity)
{
    midiBufferLock.enter();
    if (transportState.recording && transportState.playing) {
        float bps = getTempo() / 60.f;
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
		jassert(pattern.get() != nullptr);
		MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
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
	jassert(pattern.get() != nullptr);
	MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
    recordBuffer.clear();
    midiBufferLock.exit();
}

void Drums::clearTrack(int note)
{
    midiBufferLock.enter();
	jassert(pattern.get() != nullptr);
	MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
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

void Drums::resetToZero()
{
    sampleCounter = 0;
}

void Drums::prepareToPlay (int /*samplesPerBlockExpected*/, double inSampleRate)
{
    sampleRate = inSampleRate;
    midiCollector.reset (sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);

	AdjustMidiBuffers();
}

void Drums::AdjustMidiBuffers(void)
{
    midiBufferLock.enter();

    float bps = getTempo() / 60.f;
    int numBeats = 8;
    float seconds = numBeats / bps;
    float samples = (float) (sampleRate * seconds);
    maxRecordSamples = (long) samples;
    
    // Adjust the metronome buffer
    long metronomePos = 0;
    metronomeBuffer.clear();
    for (int i = 0; i < numBeats; ++i) {
        metronomeBuffer.addEvent(MidiMessage::noteOn(1, 16, 1.f), metronomePos);
        metronomePos += (long)(samples / numBeats);
    }
    
    // Adjust sample positions of all events in record buffer
	if (pattern.get() != nullptr)
	{
		pattern->Conform(getTempo(), sampleRate);
	}

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
    
    if (transportState.playing) {
        playbackState.reset();
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
		jassert(pattern.get() != nullptr);
		MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
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
        
        if (transportState.metronomeOn) {
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


void Drums::setDrumKit(SharedPtr<DrumKit> aKit)
{
	jassert(aKit.get() != nullptr);

    midiBufferLock.enter();

	synth.clearSounds();

	kit = aKit;

	int count = kit->GetSampleCount();
	numNotes = count;
	for (int i = 0; i < count; ++i)
	{
		SharedPtr<DrumSample> sample = kit->GetSample(i);
		synth.addSound(sample->GetSound());
	}
    
	// Continue to use the hardcoded clave sound for the metronome for now
	AiffAudioFormat aiffFormat;
	ScopedPointer<AudioFormatReader> clv (aiffFormat.createReaderFor (new MemoryInputStream (BinaryData::TMD_CHIL_CLV_aif,
																										BinaryData::TMD_CHIL_CLV_aifSize,
																										false),
																				true));
	BigInteger notes;
	notes.setRange (16, 1, true);
	synth.addSound (new SamplerSound ("", *clv, notes, 16, 0.0, 0.1, 10.0));

    midiBufferLock.exit();
}


void Drums::setPattern(SharedPtr<DrumPattern> aPattern)
{
	jassert(aPattern.get() != nullptr);

    midiBufferLock.enter();

	resetToZero();

	pattern = aPattern;
	setDrumKit(pattern->GetDrumKit());
	if (tempoSource == kPatternTempo)
		setTempoSlider(pattern->GetTempo());
	else
		AdjustMidiBuffers();	// Conform the buffers to the current global tempo and sample rate

    midiBufferLock.exit();
}


Drums::TransportState::TransportState(bool record, bool play, bool metronome)
: recording(record)
, playing(play)
, metronomeOn(metronome)
{
}

void Drums::TransportState::play()
{
    playing = true;
    sendChangeMessage();
}

void Drums::TransportState::pause()
{
    playing = false;
    recording = false;
    sendChangeMessage();
}

void Drums::TransportState::record(bool state)
{
    recording = state;
    if (recording && !playing)
        playing = true;
    sendChangeMessage();
}

void Drums::TransportState::metronome(bool state)
{
    metronomeOn = state;
    sendChangeMessage();
}

void Drums::TransportState::toggleRecording()
{
    record(!recording);
}

void Drums::TransportState::togglePlayback()
{
    playing ? pause() : play();
}

void Drums::TransportState::toggleMetronome()
{
    metronome(!metronomeOn);
}

void Drums::addTransportListener(ChangeListener* listener)
{
    MessageManagerLock mml;
    transportState.addChangeListener(listener);
}

void Drums::removeTransportListener(ChangeListener* listener)
{
    MessageManagerLock mml;
    transportState.removeChangeListener(listener);
}


float Drums::getTempo(void)
{
	switch (tempoSource)
	{
		default :
			jassertfalse;
			break;
		case kGlobalTempo :
			break;
		case kPatternTempo :
			if (pattern.get() != nullptr)
				return pattern->GetTempo();
			break;
	}

	return globalTempo;
}


void Drums::setTempo(float tempo)
{
	ScopedLock lock(midiBufferLock);

	switch (tempoSource)
	{
		default :
			jassertfalse;
			return;
		case kPatternTempo :
			if (pattern.get() != nullptr)
			{
				pattern->SetTempo(tempo);
				break;
			}
			// fall through
		case kGlobalTempo :
		{
			globalTempo = tempo;

			AirHarpApplication* app = AirHarpApplication::getInstance();
			ApplicationProperties& props = app->getProperties();
			props.getUserSettings()->setValue("tempo", globalTempo);
//			props.saveIfNeeded();	// Very slow to save here, save when the app shuts down, worst case
			break;
		}
	}

	AdjustMidiBuffers();
}


Drums::TempoSource Drums::getTempoSource(void)
{
	return tempoSource;
}


void Drums::setTempoSource(TempoSource source)
{
	ScopedLock lock(midiBufferLock);

	switch (source)
	{
		default :
			jassertfalse;
			// fall through
		case kGlobalTempo :
			tempoSource = kGlobalTempo;
			break;
		case kPatternTempo :
			tempoSource = kPatternTempo;
			break;
	}

	float tempo = getTempo();
	setTempoSlider(tempo);

	AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	props.getUserSettings()->setValue("tempoSource", tempoSource);
	props.saveIfNeeded();

	AdjustMidiBuffers();
}


// Handles changes to the tempo slider
void Drums::sliderValueChanged (Slider *slider)
{
	double value = slider->getValue();
	setTempo((float) value);
}


void Drums::registerTempoSlider(Slider* slider)
{
	tempoSlider = slider;
}


void Drums::setTempoSlider(float tempo)
{
	jassert(tempoSlider != nullptr);
	tempoSlider->setValue((double) tempo);
}

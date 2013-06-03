#include "Main.h"
#include "Drums.h"
#include "KitManager.h"
#include "PatternManager.h"
#include "DrumPattern.h"


Drums::Drums() :
    transportState(false,false,false,false),
    sampleCounter(0),
    maxRecordSamples(0),
    numNotes(0),
    sampleRate((double) DrumPattern::kDefaultSampleRate),
	tempoSlider(nullptr)
{
	AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	globalTempo = (float) props.getUserSettings()->getDoubleValue("tempo", (double) DrumPattern::kDefaultTempo);

	if (props.getUserSettings()->getBoolValue("tempoSource", kPatternTempo) != kGlobalTempo)
		tempoSource = kPatternTempo;
	else
		tempoSource = kGlobalTempo;
    
    bool metronome = props.getUserSettings()->getBoolValue("metronome", false);
    transportState.metronome(metronome);

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
		// Quantize the note to the nearest 16th boundary
		// Assumes pattern length is 2 bars of 4/4 time
        float bps = getTempo() / 60.0f;
        float sixteenthsPerSecond = bps * 4.0f;
        float samplesPerSixteenth = (float) (sampleRate / sixteenthsPerSecond);
        float sixteenthsIntoPattern = sampleCounter / samplesPerSixteenth;
        long quantizedPosition = (long) sixteenthsIntoPattern;
        float diff = sixteenthsIntoPattern - quantizedPosition;
        Logger::writeToLog(String::formatted("%f\n", diff));
        if (diff >= 0.5) {
            quantizedPosition++;
            if (quantizedPosition >= 32)
                quantizedPosition = 0;
        }
		quantizedPosition = (long) (quantizedPosition * samplesPerSixteenth);
        
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
		{
            recordBuffer.addEvent(m, quantizedPosition);
			pattern->SetDirty(true);
		}
        else /*if (found && position == quantizedPosition && existingMessage.getNoteNumber() == note)*/	// Redundant test
            replaceNoteVelocity(m, quantizedPosition);
        
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

void Drums::AllNotesOff(void)
{
	synth.allNotesOff(0, false);
}

void Drums::clear()
{
    midiBufferLock.enter();
	jassert(pattern.get() != nullptr);
	pattern->PrepareToModify();
	MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
    recordBuffer.clear();
	pattern->SetDirty(true);
    midiBufferLock.exit();
}

void Drums::clearTrack(int note)
{
    midiBufferLock.enter();
	jassert(pattern.get() != nullptr);
	pattern->PrepareToModify();
	MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
    MidiBuffer::Iterator i(recordBuffer);
    int samplePos = 0;
    MidiMessage message;
    MidiBuffer newBuffer;
    while (i.getNextEvent(message, samplePos))
    {
        if (message.getNoteNumber() != note)
            newBuffer.addEvent(message, samplePos);
		else
			pattern->SetDirty(true);
    }
    recordBuffer = newBuffer;
    midiBufferLock.exit();
}

void Drums::replaceNoteVelocity(MidiMessage& inMessage, int inSamplePos)
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
        if (inMessage.getNoteNumber() == message.getNoteNumber() && samplePos == inSamplePos)
        {
            message.setVelocity(inMessage.getVelocity() / 127.f);
			pattern->SetDirty(true);
        }
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
    
    double position = sampleCounter / (double)maxRecordSamples;

    float bps = getTempo() / 60.f;
    int numBeats = 8;
    float seconds = numBeats / bps;
    float samples = (float) (sampleRate * seconds);
    maxRecordSamples = (long) samples;

    sampleCounter = (long)(maxRecordSamples * position);

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
    if (!transportState.exporting)
	    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
    
    if (transportState.playing)
	{
	    if (!transportState.exporting)
		{
			playbackState.reset();
			keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
		}

		jassert(pattern.get() != nullptr);
		MidiBuffer& recordBuffer = pattern->GetMidiBuffer();
        MidiBuffer::Iterator i(recordBuffer);
        int samplePos = sampleCounter;
        MidiMessage message;
        i.setNextSamplePosition(sampleCounter);
        while (i.getNextEvent(message, samplePos) && samplePos < sampleCounter + bufferToFill.numSamples)
        {
            //Logger::writeToLog(String::formatted("%d\n", samplePos));
            incomingMidi.addEvent(message, samplePos - sampleCounter);
		    if (!transportState.exporting)
	            playbackState.noteOn(1, message.getNoteNumber(),1);
        }
        
        if (transportState.metronomeOn && !transportState.exporting) {
            MidiBuffer::Iterator metronomeIterator(metronomeBuffer);
            metronomeIterator.setNextSamplePosition(sampleCounter);
            while (metronomeIterator.getNextEvent(message, samplePos) && samplePos < sampleCounter + bufferToFill.numSamples)
            {
                //Logger::writeToLog(String::formatted("%d\n", samplePos));
                incomingMidi.addEvent(message, 0);
            }
        }

		sampleCounter += bufferToFill.numSamples;
		if (sampleCounter >= maxRecordSamples)
			sampleCounter = 0;
    }
    else
    if (!transportState.exporting)
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
    
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    
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
		for (int j = 0; j < sample->GetLayerCount(); ++j)
		{
			synth.addSound(sample->GetSound(j));
		}
	}
    
	SharedPtr<DrumSample> sample = kit->GetMetronome();
	for (int j = 0; j < sample->GetLayerCount(); ++j)
	{
		synth.addSound(sample->GetSound(j));
	}

    midiBufferLock.exit();
}


void Drums::setPattern(SharedPtr<DrumPattern> aPattern)
{
	jassert(aPattern.get() != nullptr);

    midiBufferLock.enter();

	resetToZero();

	pattern = aPattern;
	if (tempoSource == kPatternTempo) {
        setTempo(pattern->GetTempo());
		;//setTempoSlider(pattern->GetTempo());
    }
	else
		AdjustMidiBuffers();	// Conform the buffers to the current global tempo and sample rate

    midiBufferLock.exit();
}


Drums::TransportState::TransportState(bool record, bool play, bool exportState, bool metronome)
: recording(record)
, playing(play)
, exporting(exportState)
, metronomeOn(metronome)
{
}

void Drums::TransportState::play()
{
    playing = true;
	if (!exporting)
	    sendChangeMessage();
}

void Drums::TransportState::pause()
{
    playing = false;
    recording = false;
	if (!exporting)
	    sendChangeMessage();
	exporting = false;
}

void Drums::TransportState::doExport()
{
	exporting = true;
}

void Drums::TransportState::record(bool state)
{
	if (state)
	{
		Drums& drums = Drums::instance();
		SharedPtr<DrumPattern> pattern = drums.getPattern();
		jassert(pattern.get() != nullptr);
		pattern->PrepareToModify();
	}
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


float Drums::getTempo(void) const
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
				if (pattern->GetTempo() != tempo)
					pattern->PrepareToModify();
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

	//float tempo = getTempo();
	//setTempoSlider(tempo);

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
	tempoSlider->setValue((double) tempo, dontSendNotification);
}

int Drums::getCurrentStep() const
{
    float bps = getTempo() / 60.0f;
    float sixteenthsPerSecond = bps * 4.0f;
    float samplesPerSixteenth = (float) (sampleRate / sixteenthsPerSecond);
    float sixteenthsIntoPattern = sampleCounter / samplesPerSixteenth;
    return (int)sixteenthsIntoPattern;
}

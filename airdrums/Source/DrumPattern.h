/*
  ==============================================================================

    DrumPattern.h
    Created: 30 Mar 2013 12:38:40pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMPATTERN_H_2DCA5AE9__
#define __DRUMPATTERN_H_2DCA5AE9__

#include "KitManager.h"
#include "Types.h"
#include "DrumItem.h"
#include "DrumKit.h"

#include "../JuceLibraryCode/JuceHeader.h"


class DrumPattern : public DrumItem<DrumPattern>
{
public:
	using DrumItem<DrumPattern>::RevertToClean;

	enum
	{
		kDefaultTempo = 118,
		kDefaultSampleRate = 44100
	};

	DrumPattern();
	DrumPattern(const DrumPattern& source);	// Copy construct a modifiable pattern from the source
	~DrumPattern();

	// Initialization

	Status	LoadFromXml(XmlElement* element, File& directory);
	Status	SaveToXml(String fileName, File& directory);

	// This pattern object owns and manages the lifetime of the returned buffer object.
	// The caller should not store this reference, but use it only for the duration of the function in which it is obtained.
	MidiBuffer&	GetMidiBuffer(void) const;
	SharedPtr<DrumKit> GetDrumKit(void) const;
	float GetTempo(void) const;
	float GetConformTempo(void) const;
	void SetTempo(float tempo);				// Set the pattern's tempo, and conform the midi buffer to it
	void Conform(float tempo, double rate);	// Conform the pattern to the passed in (global) tempo and sample rate, also sets pattern sample rate
	void SetSampleRate(double rate);		// Set the pattern's sample rate, and conform the midi buffer to it
	double GetSampleRate(void) const;

protected:
	friend class DrumItem<DrumPattern>;
	void	RevertToClean(const DrumPattern* clean);

private:
	void Conform(MidiBuffer& buffer, float tempo, double rate);

	UniquePtr<MidiBuffer>	mMidiBuffer;
	SharedPtr<DrumKit>		mDrumKit;
	float					mTempo;			// The current local tempo of the pattern, which may or may not be the tempo it's currently conformed to
	float					mConformTempo;	// The tempo the pattern is conformed to, which may be either the local pattern tempo above or the global tempo
	double					mSampleRate;
};

#endif  // __DRUMPATTERN_H_2DCA5AE9__

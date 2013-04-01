/*
  ==============================================================================

    DrumPattern.h
    Created: 30 Mar 2013 12:38:40pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMPATTERN_H_2DCA5AE9__
#define __DRUMPATTERN_H_2DCA5AE9__

#include "../JuceLibraryCode/JuceHeader.h"

#include "Types.h"
#include "DrumItem.h"


class DrumPattern : public DrumItem
{
public:
	DrumPattern();
	~DrumPattern();

	// Initialization

	Status	LoadFromXml(XmlElement* element, File& directory);
	Status	SaveToXml(String fileName, File& directory);

	// This pattern object owns and manages the lifetime of the returned buffer object.
	// The caller should not store this reference, but use it only for the duration of the function in which it is obtained.
	MidiBuffer&	GetMidiBuffer(void);

private:
	UniquePtr<MidiBuffer>	mMidiBuffer;
};

#endif  // __DRUMPATTERN_H_2DCA5AE9__

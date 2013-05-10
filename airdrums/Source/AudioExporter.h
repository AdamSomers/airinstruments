/*
  ==============================================================================

    AudioExporter.h
    Created: 8 May 2013 5:03:18pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __AUDIOEXPORTER_H_99CEB6B6__
#define __AUDIOEXPORTER_H_99CEB6B6__

#include "../JuceLibraryCode/JuceHeader.h"


class AudioExporter
{
public:
	AudioExporter(DocumentWindow* mainWindow);
	~AudioExporter();

	void Export(void);

private:
	DocumentWindow* mMainWindow;
};


#endif  // __AUDIOEXPORTER_H_99CEB6B6__

/*
  ==============================================================================

    AudioSettingsDialog.h
    Created: 21 Mar 2013 2:14:27pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __AUDIOSETTINGSDIALOG_H_33FC3FE0__
#define __AUDIOSETTINGSDIALOG_H_33FC3FE0__

#include "../JuceLibraryCode/JuceHeader.h"


class AudioSettingsDialog :	public DialogWindow
{
public:

    AudioSettingsDialog (DocumentWindow* window, AudioDeviceManager& manager, ApplicationProperties& properties);
	virtual ~AudioSettingsDialog();

    void closeButtonPressed();

	static String getPropertiesName(void);

private:
	AudioDeviceSelectorComponent* selector;
	AudioDeviceManager& devManager;
	ApplicationProperties& appProperties;
	WeakReference<AudioSettingsDialog>::Master masterReference;
	friend class WeakReference<AudioSettingsDialog>;
};

#endif  // __AUDIOSETTINGSDIALOG_H_33FC3FE0__

/*
  ==============================================================================

    AudioSettingsDialog.cpp
    Created: 21 Mar 2013 2:14:15pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "AudioSettingsDialog.h"


AudioSettingsDialog::AudioSettingsDialog (DocumentWindow* window, AudioDeviceManager& manager, ApplicationProperties& properties)
    : DialogWindow ("Audio Settings", Colours::azure, false, true)
	, selector(nullptr)
	, devManager(manager)
	, appProperties(properties)
{
	selector = new AudioDeviceSelectorComponent(manager, 0, 0, 2, 2, false, false, true, true);
	selector->setSize (500, 450);
    setUsingNativeTitleBar (true);
    setContentNonOwned (selector, true);
    centreAroundComponent (window, getWidth(), getHeight());
    setResizable (false, false);
	setVisible(true);
	toFront(true);
}


AudioSettingsDialog::~AudioSettingsDialog()
{
	XmlElement* audioState = devManager.createStateXml();
	appProperties.getUserSettings()->setValue(getPropertiesName(), audioState);
	if (audioState != nullptr)
		delete audioState;
	appProperties.saveIfNeeded();

	if (selector != nullptr)
		delete selector;
	masterReference.clear();
}


void AudioSettingsDialog::closeButtonPressed()
{
    setVisible (false);
	delete this;
}


String AudioSettingsDialog::getPropertiesName(void)
{
	return "audioDeviceManager";
}

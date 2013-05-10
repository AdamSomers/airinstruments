/*
  ==============================================================================

    AudioExporter.cpp
    Created: 8 May 2013 5:03:08pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "ExportComponent.h"
#include "AudioExporter.h"

#include "../JuceLibraryCode/JuceHeader.h"


AudioExporter::AudioExporter(DocumentWindow* mainWindow) : mMainWindow(mainWindow)
{
}


AudioExporter::~AudioExporter()
{
}


void AudioExporter::Export(void)
{
	ExportComponent* component = new ExportComponent();
	DialogWindow::LaunchOptions options;
	options.dialogTitle = "Export As Audio";
	options.dialogBackgroundColour = Colours::azure;
	options.content.set(component, true);
	options.componentToCentreAround = mMainWindow;
	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = true;
	options.resizable = false;
	options.useBottomRightCornerResizer = false;
	UniquePtr<DialogWindow> dialog(options.create());
	component->SetParentDialog(dialog.get());
	dialog->runModalLoop();
}

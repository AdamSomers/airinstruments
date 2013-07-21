/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "Main.h"

void AirHarpApplication::initialise (const String& commandLine)
{
	PropertiesFile::Options options;
	options.applicationName = "AirHarp";
	options.filenameSuffix = ".settings";
	options.folderName = "AirHarp";
	options.osxLibrarySubFolder = "Application Support";
	options.commonToAllUsers = "false";
	options.ignoreCaseOfKeyNames = true;
	options.millisecondsBeforeSaving = 1000;
	options.storageFormat = PropertiesFile::storeAsXML;
	properties.setStorageParameters(options);

    mainWindow = new MainWindow();
    
    XmlElement* audioState = properties.getUserSettings()->getXmlValue(AudioSettingsDialog::getPropertiesName());
    String audioStatus = audioDeviceManager.initialise (0, 2, audioState, true, String::empty, 0);
	if (audioState != nullptr)
		delete audioState;
	if (audioStatus != "")
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Audio device error", audioStatus);
        quit();
        return;
    }

    audioDeviceManager.addAudioCallback(this);
    
	mainMenu = new MainMenu();
#if JUCE_WINDOWS
    mainWindow->setMenuBar(mainMenu);
#elif JUCE_MAC
    MenuBarModel::setMacMainMenu(mainMenu);
#endif
    
    commandManager.registerAllCommandsForTarget (mainWindow);
    mainMenu->setApplicationCommandManagerToWatch (&commandManager);
}

void AirHarpApplication::shutdown()
{
    properties.saveIfNeeded();
    
	if (settingsDialog != nullptr)
		delete settingsDialog;
    
#if JUCE_WINDOWS
    mainWindow->setMenuBar(nullptr);
#elif JUCE_MAC
    MenuBarModel::setMacMainMenu(nullptr);
#endif
	delete mainMenu;
    
    Logger::writeToLog("AirBeats Shutdown");
    Logger::setCurrentLogger(nullptr);

    // Add your application's shutdown code here..

    mainWindow = nullptr; // (deletes our window)
    audioDeviceManager.removeAudioCallback(this);
    MotionDispatcher::destruct();
}

//==============================================================================
void AirHarpApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
}

void AirHarpApplication::audioDeviceAboutToStart (AudioIODevice* device)
{
    AudioServer::GetInstance()->SetFs(device->getCurrentSampleRate());
}
void AirHarpApplication::audioDeviceStopped()
{
    
}
void AirHarpApplication::audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                            float** outputChannelData, int numOutputChannels, int numSamples)
{
    AudioServer::GetInstance()->SetInputChannels(numInputChannels);
    AudioServer::GetInstance()->SetOutputChannels(numOutputChannels);
    AudioServer::GetInstance()->AudioServerCallback(inputChannelData,
                                                    outputChannelData,
                                                    numSamples);
}

void AirHarpApplication::anotherInstanceStarted (const String& commandLine)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

void AirHarpApplication::showAudioSettingsDlg()
{
    settingsDialog = new AudioSettingsDialog(mainWindow, audioDeviceManager, properties);
}

void AirHarpApplication::enterFullscreenMode()
{
   //On Windows, going to kiosk mode still shows toolbars and taskbar if the window is resizable.
   //Changing resizability causes a crash because MainComponent doesn't properly handle a reset
   // of the OpenGL context.  For now we will live with not-quite fullscreen
   //mainWindow->setResizable(false,false);
    //mainWindow->setFullScreen(true);
#if JUCE_MAC
    Desktop::getInstance().setKioskModeComponent(mainWindow, false);
#endif
}

void AirHarpApplication::exitFullscreenMode()
{
    //mainWindow->setFullScreen(false);
#if JUCE_MAC
    Desktop::getInstance().setKioskModeComponent(nullptr);
#endif
   //mainWindow->setResizable(true,false);
}

bool AirHarpApplication::isFullscreen() const
{
#if JUCE_MAC
    return Desktop::getInstance().getKioskModeComponent() != nullptr;
#else
    return mainWindow->isFullScreen();
#endif
}

AirHarpApplication::MainWindow::MainWindow()  : DocumentWindow ("AirHarp",
                                Colours::lightgrey,
                                DocumentWindow::allButtons)
{
    setContentOwned (new MainContentComponent(), true);
    centreWithSize (getWidth(), getHeight());
    setUsingNativeTitleBar(true);
    setResizable(true, false);
    setResizeLimits(800, 600, 3840, 1800);
    setVisible (true);
#if JUCE_MAC
    setFullScreen(true);
    Desktop::getInstance().setKioskModeComponent(this, false);
#else
    setFullScreen(true);
#endif
}

void AirHarpApplication::MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

void AirHarpApplication::MainWindow::getCommandInfo (CommandID commandID, ApplicationCommandInfo &result)
{
    switch (commandID)
    {
        case kAudioSettingsCmd:
            result.setInfo ("Audio Settings", "Change audio configuration settings", "Options", 0);
            result.setActive(true);
            result.addDefaultKeypress (',', ModifierKeys::commandModifier);
            break;
        case kMoreInfoCmd:
            result.setInfo ("More Info...", "Visit Handwavy Website", "File", 0);
            result.setActive(true);
            break;
        case kFullscreenCmd:
            result.setInfo ("Toggle Fullscreen Mode", "Toggle Fullscreen Mode", "Options", 0);
            result.setActive(true);
            break;
        default:
            break;
    }
}

void AirHarpApplication::MainWindow::getAllCommands (Array< CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        kAudioSettingsCmd,
        kMoreInfoCmd,
        kFullscreenCmd
    };
    
    commands.addArray (ids, numElementsInArray (ids));
}

bool AirHarpApplication::MainWindow::perform (const InvocationInfo &info)
{
    switch(info.commandID)
    {
        default :
        {
            //jassertfalse;
            break;
        }
            
        case kAudioSettingsCmd :
        {
            if (AirHarpApplication::getInstance()->settingsDialog != nullptr)
                break;
            
            AirHarpApplication::getInstance()->showAudioSettingsDlg();
            
            break;
        }
        
        case kMoreInfoCmd:
        {
            URL url("http://handwavy.com");
            url.launchInDefaultBrowser();
            break;
        }
            
        case kFullscreenCmd:
        {
            if (AirHarpApplication::getInstance()->isFullscreen())
                AirHarpApplication::getInstance()->exitFullscreenMode();
            else
                AirHarpApplication::getInstance()->enterFullscreenMode();
        }
    }
    return true;
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AirHarpApplication)

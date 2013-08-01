/*
  ==============================================================================

    This file was auto-generated by the Introjucer!
	(and heavily hand edited afterwards)

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "Main.h"
#include "KitManager.h"
#include "PatternManager.h"
#include "AudioExporter.h"
#if JUCE_WINDOWS
#if JUCE_DEBUG
    //#include <vld.h>
#endif
#endif


AirHarpApplication::AirHarpApplication()
{
#if 0
    for (int i = 0; i < 16; ++i)
        Logger::writeToLog(Uuid::Uuid().toString());
#endif
    fileLogger = FileLogger::createDateStampedLogger("AirBeats", "AirBeats", ".txt", "AirBeats Log - please send this file to info@handwavy.co if you have encountered an error.  Thanks!");
    Logger::setCurrentLogger(fileLogger);
    Logger::writeToLog("AirHarpApplication instantiated");
}


void AirHarpApplication::initialise (const String& /*commandLine*/)
{
    // This method is where you should put your application's initialisation code..
    
    

	PropertiesFile::Options options;
	options.applicationName = "AirBeats";
	options.filenameSuffix = ".settings";
	options.folderName = "AirBeats";
	options.osxLibrarySubFolder = "Application Support";
	options.commonToAllUsers = "false";
	options.ignoreCaseOfKeyNames = true;
	options.millisecondsBeforeSaving = 1000;
	options.storageFormat = PropertiesFile::storeAsXML;
	properties.setStorageParameters(options);

    mainWindow = new MainWindow();
	mainMenu = new MainMenu();
	#if JUCE_WINDOWS
		mainWindow->setMenuBar(mainMenu);
	#elif JUCE_MAC
		MenuBarModel::setMacMainMenu(mainMenu);
	#endif
    
    commandManager.registerAllCommandsForTarget (mainWindow);
    mainMenu->setApplicationCommandManagerToWatch (&commandManager);
    
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
    
    

    postMessage(new InitializeMessage);

//    mainWindow->getContentComponent()->grabKeyboardFocus();
#if JUCE_MAC
    postMessage(new GrabFocusMessage);
#endif
    Logger::writeToLog("AirHarpApplication initialized");
}

void AirHarpApplication::shutdown()
{
    // Add your application's shutdown code here..

	properties.saveIfNeeded();

	if (settingsDialog != nullptr)
		delete settingsDialog;

	StopAudioDevice();

	PatternManager::Destruct();
	#if JUCE_WINDOWS
		mainWindow->setMenuBar(nullptr);
	#elif JUCE_MAC
		MenuBarModel::setMacMainMenu(nullptr);
	#endif
	delete mainMenu;

	MotionDispatcher::instance().stop();
    mainWindow = nullptr; // (deletes our window)
    MotionDispatcher::destruct();

    //audioDeviceManager.removeAudioCallback(this);
	KitManager::Destruct();
    
    Logger::writeToLog("AirBeats Shutdown");
    Logger::setCurrentLogger(nullptr);
}


void AirHarpApplication::StartAudioDevice(void)
{
    audioDeviceManager.addAudioCallback (&audioSourcePlayer);
}


void AirHarpApplication::StopAudioDevice(void)
{
    audioDeviceManager.removeAudioCallback (&audioSourcePlayer);
}


//==============================================================================
void AirHarpApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
}

void AirHarpApplication::audioDeviceAboutToStart (AudioIODevice* /*device*/)
{
        
}
void AirHarpApplication::audioDeviceStopped()
{
        
}
void AirHarpApplication::audioDeviceIOCallback (const float** /*inputChannelData*/, int /*numInputChannels*/,
                            float** /*outputChannelData*/, int /*numOutputChannels*/, int /*numSamples*/)
{
}
    
void AirHarpApplication::anotherInstanceStarted (const String& /*commandLine*/)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

void AirHarpApplication::showAudioSettingsDlg()
{
    settingsDialog = new AudioSettingsDialog(mainWindow, audioDeviceManager, properties);
}

void AirHarpApplication::MainWindow::getAllCommands (Array< CommandID>& commands)
{
    // this returns the set of all commands that this target can perform..
    const CommandID ids[] = {
        kAudioSettingsCmd,
        kSavePatternAsCmd,
        kLoadPatternCmd,
        kUsePatternTempoCmd,
        kExportCmd,
        kNewPatternCmd,
        kSavePatternCmd,
        kDeletePatternCmd,
        kMoreInfoCmd,
        kFullscreenCmd,
        kAdvancedModeCmd
    };
    
    commands.addArray (ids, numElementsInArray (ids));

}

void AirHarpApplication::MainWindow::getCommandInfo (CommandID commandID, ApplicationCommandInfo &result)
{
    AirHarpApplication* app = AirHarpApplication::getInstance();
    ApplicationProperties& props = app->getProperties();
    switch (commandID)
    {
        case kNewPatternCmd:
            result.setInfo ("New Pattern", "Create a new pattern", "File", 0);
            result.setActive(true);
            result.addDefaultKeypress ('N', ModifierKeys::commandModifier);
            break;
        case kLoadPatternCmd:
            result.setInfo ("Load Pattern", "Load a pattern", "File", 0);
            result.setActive(true);
            result.addDefaultKeypress ('O', ModifierKeys::commandModifier);
            break;
        case kSavePatternCmd:
        {
            result.setInfo ("Save Pattern", "Save the current pattern", "File", 0);
            SharedPtr<DrumPattern> pattern = Drums::instance().getPattern();
            result.setActive(pattern != nullptr && pattern->GetModifiable());
            result.addDefaultKeypress ('S', ModifierKeys::commandModifier);
        }
            break;
        case kSavePatternAsCmd:
            result.setInfo ("Save Pattern As...", "Save the current pattern as a copy", "File", 0);
            result.setActive(true);
            result.addDefaultKeypress ('S', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
            break;
        case kExportCmd:
            result.setInfo ("Export Pattern...", "Export pattern as audio", "File", 0);
            result.setActive(true);
            break;
        case kDeletePatternCmd:
        {
            result.setInfo ("Delete Pattern", "Delete the active pattern", "File", 0);
            Drums& drums = Drums::instance();
            SharedPtr<DrumPattern> pattern = drums.getPattern();
            result.setActive(pattern.get() != nullptr && pattern->GetModifiable());
        }
            break;
        case kUsePatternTempoCmd:
        {
            result.setInfo ("Use Pattern Tempo", "Use BPM from pattern file, or global tempo", "Options", 0);
            result.setActive(true);
            bool tick = false;
            if (props.getUserSettings()->getBoolValue("tempoSource", Drums::kGlobalTempo) != Drums::kGlobalTempo)
                tick = true;
            else
                tick = false;
            result.setTicked(tick);
            result.addDefaultKeypress ('T', ModifierKeys::commandModifier);
        }
            break;
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
        case kAdvancedModeCmd:
        {
            result.setInfo ("Advanced Mode", "Toggle Advanced Mode", "Options", 0);
            result.setActive(true);
            bool tick = false;
            if (props.getUserSettings()->getBoolValue("advancedMode", false))
                tick = true;
            result.setTicked(tick);
        }
            break;

    }
}

bool AirHarpApplication::MainWindow::perform (const InvocationInfo &info)
{
    AirHarpApplication* app = AirHarpApplication::getInstance();
    ApplicationProperties& props = app->getProperties();

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
		case kSavePatternAsCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.SavePatternAs();
            
            
            
            
			break;
		}
		case kLoadPatternCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.LoadPattern();
            MainContentComponent* content = dynamic_cast<MainContentComponent*>(this->getContentComponent());
            if (content)
                content->postMessage(new PatternChangedMessage);
			break;
		}
		case kUsePatternTempoCmd :
		{
            bool usePatternTempo = !(props.getUserSettings()->getBoolValue("tempoSource", Drums::kGlobalTempo) != Drums::kGlobalTempo);
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.UsePatternTempo(usePatternTempo);
            MainContentComponent* content = dynamic_cast<MainContentComponent*>(this->getContentComponent());
            if (content)
                content->postMessage(new MainContentComponent::TempoSourceChangedMessage);
			break;
		}
		case kExportCmd :
		{
			UniquePtr<AudioExporter> exporter(new AudioExporter(this));
			exporter->Export();
			break;
		}
		case kNewPatternCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.CreateNewPattern();
			break;
		}
		case kSavePatternCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.SavePattern();
			break;
		}
        case kDeletePatternCmd:
        {
            PatternManager& mgr = PatternManager::GetInstance();
            /*PatternManager::Status status =*/ mgr.DeletePattern();
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
        case kAdvancedModeCmd:
        {
            bool advancedMode = props.getUserSettings()->getBoolValue("advancedMode", false);
            props.getUserSettings()->setValue("advancedMode", !advancedMode);
        }
            break;
	}
    
//#if JUCE_MAC
//    MenuBarModel::setMacMainMenu(mainMenu);
//#endif
    
	return true;
}

void AirHarpApplication::handleMessage(const juce::Message& m)
{
    Message* inMsg = const_cast<Message*>(&m);

#if defined(JUCE_MAC) && !defined(JUCE_DEBUG)
    GrabFocusMessage* grabFocusMessage = dynamic_cast<GrabFocusMessage*>(inMsg);
    if (grabFocusMessage)
    {
        mainWindow->getContentComponent()->grabKeyboardFocus();
        if(!mainWindow->getContentComponent()->hasKeyboardFocus(false))
            postMessage(inMsg);
    }
#endif
    
    InitializeMessage* initializeMessage = dynamic_cast<InitializeMessage*>(inMsg);
    if (initializeMessage)
    {
        //audioDeviceManager.addAudioCallback(this);
        audioSourcePlayer.setSource (&Drums::instance());
        StartAudioDevice();
        Logger::writeToLog(audioDeviceManager.getCurrentAudioDevice()->getName());
        
        PatternManager& pmgr = PatternManager::GetInstance();
        /*PatternManager::Status pstatus =*/ pmgr.BuildPatternList();
        
        //Drums::instance().setPattern(SharedPtr<DrumPattern>(new DrumPattern));	// No longer a need to start with an empty pattern, use the last used pattern from the prefs
        
        

        String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
        String kitName = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitName", "Default");
        if (kitUuidString == "Default")
            AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("kitUuid", KitManager::GetInstance().GetItem(0)->GetUuid().toString());
        else {
            Uuid kitUuid(kitUuidString);
            SharedPtr<DrumKit> kit = KitManager::GetInstance().GetItem(kitUuid);
            if (!kit) {
                Logger::writeToLog("Did not find saved kit with name " + kitName + "and uuid " + kitUuidString);
                AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("kitUuid", KitManager::GetInstance().GetItem(0)->GetUuid().toString());
            }
        }
    }
}

void AirHarpApplication::enterFullscreenMode()
{
    //On Windows, going to kiosk mode still shows toolbars and taskbar if the window is resizable.
    //Changing resizability causes a crash because MainComponent doesn't properly handle a reset
    // of the OpenGL context.  For now we will live with not-quite fullscreen
    //mainWindow->setResizable(false,false);
#if JUCE_MAC
    Desktop::getInstance().setKioskModeComponent(mainWindow, false);
#else
    mainWindow->setFullScreen(true);
#endif
    ((MainContentComponent*)mainWindow->getContentComponent())->showFullscreenTip();
}

void AirHarpApplication::exitFullscreenMode()
{
#if JUCE_MAC
    Desktop::getInstance().setKioskModeComponent(nullptr);
#else
    mainWindow->setFullScreen(false);
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

//==============================================================================
AirHarpApplication::MainWindow::MainWindow()  :
								DocumentWindow ("AirBeats",
                                Colours::lightgrey,
                                DocumentWindow::allButtons)
{
    setContentOwned (new MainContentComponent(), true);
    centreWithSize (getWidth(), getHeight());
#if JUCE_WIN
    setVisible (true);
#endif
    setFullScreen(true);
    setUsingNativeTitleBar(true);
    setResizable(true, false);
    setResizeLimits(800, 600, 3840, 1800);
#if JUCE_MAC
    setVisible (true);
    Desktop::getInstance().setKioskModeComponent(this, false);
#endif
    addKeyListener (AirHarpApplication::getInstance()->commandManager.getKeyMappings());
}

void AirHarpApplication::MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    JUCEApplication::getInstance()->systemRequestedQuit();
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (AirHarpApplication)

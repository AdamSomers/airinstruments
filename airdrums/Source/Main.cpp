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
    
    Time t = Time::getCurrentTime();
    Time thresh(2013, 6, 22, 0, 0); // Month is zero-indexed
    
    if (t > thresh)
    {
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, "v" + String(ProjectInfo::versionString) + " Expired", "Thank you for testing this build");
        quit();
        return;
    }
    
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

bool AirHarpApplication::perform (const InvocationInfo &info)
{
	switch(info.commandID)
	{
		default :
		{
			jassertfalse;
			break;
		}

		case MainMenu::kAudioSettingsCmd :
		{
			if (settingsDialog != nullptr)
				break;

			settingsDialog = new AudioSettingsDialog(mainWindow, audioDeviceManager, properties);

			break;
		}
		case MainMenu::kSavePatternAsCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.SavePatternAs();
			break;
		}
		case MainMenu::kLoadPatternCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.LoadPattern();
			break;
		}
		case MainMenu::kUsePatternTempoCmd :
		{
			bool usePatternTempo = mainMenu->GetUsePatternTempo();
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.UsePatternTempo(usePatternTempo);
			break;
		}
		case MainMenu::kExportCmd :
		{
			UniquePtr<AudioExporter> exporter(new AudioExporter(mainWindow));
			exporter->Export();
			break;
		}
		case MainMenu::kNewPatternCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.CreateNewPattern();
			break;
		}
		case MainMenu::kSavePatternCmd :
		{
			PatternManager& mgr = PatternManager::GetInstance();
			/*PatternManager::Status status =*/ mgr.SavePattern();
			break;
		}
	}

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

//==============================================================================
AirHarpApplication::MainWindow::MainWindow()  :
								DocumentWindow ("AirBeats",
                                Colours::lightgrey,
                                DocumentWindow::allButtons)
{
    setContentOwned (new MainContentComponent(), true);

    centreWithSize (getWidth(), getHeight());
    setVisible (true);
    setUsingNativeTitleBar(true);
    setResizable(true, false);
    setResizeLimits(640, 480, 3840, 1800);
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

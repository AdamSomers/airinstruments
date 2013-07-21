//
//  Main.h
//  AirHarp
//
//  Created by Adam Somers on 7/11/13.
//
//

#ifndef AirHarp_Main_h
#define AirHarp_Main_h

#include "MotionServer.h"
#include "MainComponent.h"
#include "AudioSettingsDialog.h"
#include "MainMenu.h"

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class AirHarpApplication  : public JUCEApplication
, public AudioIODeviceCallback
{
public:
    //==============================================================================
    AirHarpApplication() {}
    
    const String getApplicationName()       { return ProjectInfo::projectName; }
    const String getApplicationVersion()    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()       { return true; }
    ApplicationProperties& getProperties()		{ return properties; }
	static AirHarpApplication* getInstance()	{ return dynamic_cast<AirHarpApplication*> (JUCEApplication::getInstance()); }
    ApplicationCommandManager* getCommandManager() { return &commandManager; }

    
    //==============================================================================
    void initialise (const String& commandLine);
    
    void shutdown();
    //==============================================================================
    void systemRequestedQuit();
    
    void audioDeviceAboutToStart (AudioIODevice* device);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels, int numSamples);
    
    void anotherInstanceStarted (const String& commandLine);
    
    void showAudioSettingsDlg();

    void enterFullscreenMode();
    void exitFullscreenMode();
    bool isFullscreen() const;
    
    //==============================================================================
    /*
     This class implements the desktop window that contains an instance of
     our MainContentComponent class.
     */
    class MainWindow    : public DocumentWindow
    , public ApplicationCommandTarget
    {
    public:
        MainWindow();
        
        void closeButtonPressed();
        
        enum CommandIDs
        {
            kAudioSettingsCmd = 1,
            kMoreInfoCmd,
            kFullscreenCmd,
        };
        
        void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result);
        void getAllCommands (Array< CommandID>& commands);
        ApplicationCommandTarget * 	getNextCommandTarget () { return nullptr; }
        
        bool perform (const InvocationInfo &info);
        
        
        /* Note: Be careful if you override any DocumentWindow methods - the base
         class uses a lot of them, so by overriding you might break its functionality.
         It's best to do all your work in your content component instead, but if
         you really have to override any DocumentWindow methods, make sure your
         subclass also calls the superclass's method.
         */
        
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };
    
private:
    ScopedPointer<MainWindow> mainWindow;
    AudioDeviceManager audioDeviceManager;
    ApplicationProperties properties;
	MainMenu* mainMenu;
	WeakReference<AudioSettingsDialog> settingsDialog;
    ScopedPointer<FileLogger> fileLogger;
    ApplicationCommandManager commandManager;
};

#endif

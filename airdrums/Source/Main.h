#ifndef h_Main
#define h_Main

#include "MainComponent.h"
#include "MotionServer.h"
#include "MainMenu.h"
#include "AudioSettingsDialog.h"

//==============================================================================
class AirHarpApplication  : public JUCEApplication
                          , public MessageListener
                          //, public AudioIODeviceCallback
{
public:
    //==============================================================================
    AirHarpApplication();

    const String getApplicationName()			{ return ProjectInfo::projectName; }
    const String getApplicationVersion()		{ return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed()			{ return true; }
	ApplicationProperties& getProperties()		{ return properties; }
	static AirHarpApplication* getInstance()	{ return dynamic_cast<AirHarpApplication*> (JUCEApplication::getInstance()); }

	void StartAudioDevice(void);
	void StopAudioDevice(void);

    //==============================================================================
    void initialise (const String& /*commandLine*/);

    void shutdown();

    //==============================================================================
    void systemRequestedQuit();

    void audioDeviceAboutToStart (AudioIODevice* /*device*/);
    void audioDeviceStopped();
    void audioDeviceIOCallback (const float** /*inputChannelData*/, int /*numInputChannels*/,
                                float** /*outputChannelData*/, int /*numOutputChannels*/, int /*numSamples*/);
    
    void anotherInstanceStarted (const String& /*commandLine*/);

    void handleMessage(const juce::Message &m);
    
    void enterFullscreenMode();
    void exitFullscreenMode();
    bool isFullscreen() const;

    ApplicationCommandManager commandManager;
    
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

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */
        
        enum CommandIDs
        {
            kAudioSettingsCmd = 1,
            kSavePatternAsCmd = 2,
            kLoadPatternCmd = 3,
            kUsePatternTempoCmd = 4,
            kExportCmd = 5,
            kNewPatternCmd = 6,
            kSavePatternCmd = 7,
            kDeletePatternCmd = 8,
            kMoreInfoCmd,
            kFullscreenCmd
        };
        
        void getCommandInfo (CommandID commandID, ApplicationCommandInfo &result);
        void getAllCommands (Array< CommandID>& commands);
        ApplicationCommandTarget * 	getNextCommandTarget () { return nullptr; }
        bool perform (const InvocationInfo &info);
        

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    class PatternAddedMessage : public Message {};

    class PatternDeletedMessage : public Message {};
    
    class PatternChangedMessage : public Message {};
    
    class GrabFocusMessage : public Message {};
    
    class InitializeMessage : public Message {};

	MainWindow* GetMainWindow()					{ return mainWindow; }
    
    void showAudioSettingsDlg();

private:

    ScopedPointer<MainWindow> mainWindow;
    AudioDeviceManager audioDeviceManager;
    AudioSourcePlayer audioSourcePlayer;
    ApplicationProperties properties;
	MainMenu* mainMenu;
	WeakReference<AudioSettingsDialog> settingsDialog;
    ScopedPointer<FileLogger> fileLogger;
};

#endif

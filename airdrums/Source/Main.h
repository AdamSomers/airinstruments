#ifndef h_Main
#define h_Main

#include "MainComponent.h"
#include "MotionServer.h"
#include "MainMenu.h"
#include "AudioSettingsDialog.h"

#if defined(TIMEBOMB)

#define BETA_CHECK(month,day) \
{ \
 Time t = Time::getCurrentTime(); \
 Time thresh(2013, month, day, 0, 0); \
 if (t > thresh) \
 { \
    AlertWindow::showMessageBox(AlertWindow::WarningIcon, "v" + String(ProjectInfo::versionString) + " Expired", "Thank you for testing this build"); \
    AirHarpApplication::getInstance()->quit(); \
    return; \
} \
}

#define BETA_CHECK_RET(month,day) \
{ \
Time t = Time::getCurrentTime(); \
Time thresh(2013, month, day, 0, 0); \
if (t > thresh) \
{ \
AlertWindow::showMessageBox(AlertWindow::WarningIcon, "v" + String(ProjectInfo::versionString) + " Expired", "Thank you for testing this build"); \
AirHarpApplication::getInstance()->quit(); \
return 0; \
} \
}

#define BETA_CHECK_RANDOM_2013() \
{ \
int month = Random::getSystemRandom().nextInt(5) + 7; \
int day = Random::getSystemRandom().nextInt() % 31; \
Time t = Time::getCurrentTime(); \
Time thresh(2013, month, day, 0, 0); \
if (t > thresh) \
{ \
AlertWindow::showMessageBox(AlertWindow::WarningIcon, "v" + String(ProjectInfo::versionString) + " Expired r13" + String(month) + String(day), "Thank you for testing this build"); \
AirHarpApplication::getInstance()->quit(); \
return; \
} \
}

#define BETA_CHECK_RANDOM_2014() \
{ \
int month = Random::getSystemRandom().nextInt(12); \
int day = Random::getSystemRandom().nextInt() % 31; \
Time t = Time::getCurrentTime(); \
Time thresh(2014, month, day, 0, 0); \
if (t > thresh) \
{ \
AlertWindow::showMessageBox(AlertWindow::WarningIcon, "v" + String(ProjectInfo::versionString) + " Expired", "Thank you for testing this build"); \
AirHarpApplication::getInstance()->quit(); \
return; \
} \
}

#else
#define BETA_CHECK(month,day)
#define BETA_CHECK_RET(month,day)
#define BETA_CHECK_RANDOM_2013() 
#define BETA_CHECK_RANDOM_2014()
#endif

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
    
    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow    : public DocumentWindow
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

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    class PatternAddedMessage : public Message {};
    
    class GrabFocusMessage : public Message {};
    
    class InitializeMessage : public Message {};

	MainWindow* GetMainWindow()					{ return mainWindow; }

private:
	bool perform (const InvocationInfo &info);

    ScopedPointer<MainWindow> mainWindow;
    AudioDeviceManager audioDeviceManager;
    AudioSourcePlayer audioSourcePlayer;
    ApplicationProperties properties;
	MainMenu* mainMenu;
	WeakReference<AudioSettingsDialog> settingsDialog;
    ScopedPointer<FileLogger> fileLogger;
};

#endif

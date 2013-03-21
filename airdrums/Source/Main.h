#ifndef h_Main
#define h_Main

#include "MainComponent.h"
#include "MotionServer.h"
#include "MainMenu.h"
#include "AudioSettingsDialog.h"

//==============================================================================
class AirHarpApplication  : public JUCEApplication
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

private:
	bool perform (const InvocationInfo &info);

    ScopedPointer<MainWindow> mainWindow;
    AudioDeviceManager audioDeviceManager;
    AudioSourcePlayer audioSourcePlayer;
    ApplicationProperties properties;
	MainMenu mainMenu;
	WeakReference<AudioSettingsDialog> settingsDialog;
};

#endif

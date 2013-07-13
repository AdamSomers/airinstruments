/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_115FADC5__
#define __MAINCOMPONENT_H_115FADC5__

#include "HarpHUD.h"
#include "StringView.h"
#include "HarpView.h"
#include "TutorialSlide.h"
#include "SettingsScreen.h"

#include "../JuceLibraryCode/JuceHeader.h"



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
                               public OpenGLRenderer,
                               public ChangeListener,
                               public Leap::Listener,
                               public MultiTimer,
                               public ActionListener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    // component overrides
    void paint (Graphics&);
    void resized();
    void focusGained(FocusChangeType cause);
    void focusLost(FocusChangeType cause);
    void mouseMove(const MouseEvent& e);
    void mouseDown(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
    bool keyPressed(const KeyPress& kp);
    
    // OpenGLRenderer overrides
    void newOpenGLContextCreated();
    void renderOpenGL();
    void openGLContextClosing();
    
    // ChangeListener override
    void changeListenerCallback (ChangeBroadcaster* source);
    
    void onFrame(const Leap::Controller&);
    
    void timerCallback(int timeId);
    
    enum TimerIds
    {
        kTimerCheckLeapConnection = 0
    };
    
    void actionListenerCallback(const String& message);
private:
    void go2d();
    void go3d();
    void layoutStrings();
    void layoutChordRegions();
    bool chordRegionsNeedUpdate;
    
    void handleTapGesture(const Leap::Pointable& p);
    
    TutorialSlide* slide;
    HarpToolbar* toolbar;
    StatusBar* statusBar;
    SettingsScreen* settingsScreen;
    HUDView* leapDisconnectedView;
    View2d* backgroundView;
    std::vector<ChordRegion*> chordRegions;
    std::vector<HarpView*> harps;
    std::vector<HarpView*> inactiveHarps;
    std::vector<HUDView*> views;

    View2d fingersImage;
    int bloomShaderId;
    int shaderId;
    
    bool sizeChanged;
    
    Time lastFrame;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_115FADC5__

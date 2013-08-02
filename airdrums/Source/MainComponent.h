/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_115FADC5__
#define __MAINCOMPONENT_H_115FADC5__

#include "MainView.h"
#include "DrumsHUD.h"
#include "PadView.h"
#include "Leap.h"
#include "PlayArea.h"
#include "TrigView.h"
#include "DrumSelector.h"
#include "SkinManager.h"
#include "ListSelector.h"
#include "TutorialSlide.h"
#include "TempoControl.h"
#include "StrikeDetector.h"
#include "CursorView.h"
#include "ButtonBar.h"
#include "stickView.h"

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
                               public OpenGLRenderer,
                               public MidiKeyboardStateListener,
                               public Leap::Listener,
                               public ChangeListener,
                               public MultiTimer,
                               public MessageListener,
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
    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel);
    bool keyPressed(const KeyPress& kp);
    
    // OpenGLRenderer overrides
    void newOpenGLContextCreated();
    void renderOpenGL();
    void openGLContextClosing();
    
    // MidiKeyboardStateListener overrides
    void handleNoteOn (MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity);
    void handleNoteOff (MidiKeyboardState* /*source*/, int /*midiChannel*/, int /*midiNoteNumber*/) {}
    
    // DrumSelector::Listener override
    void drumSelectorChanged(int selectedItem);
    
    // ChangeListener override
    void changeListenerCallback(ChangeBroadcaster *source);

    // Leap::Listener override
    virtual void onFrame(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    
    // MultiTimer override
    void timerCallback(int timerId);
    
    // MessageListener override
    void handleMessage(const Message& m);
    
    // ActionListener override
    void actionListenerCallback(const String& message);
    
    class TempoSourceChangedMessage : public Message {};

    void showFullscreenTip();
private:
    void layoutPadsGrid();
    void layoutPadsLinear();
    void handleGestures(const Leap::GestureList& gestures);
    void handleHands(const Leap::HandList& hands);
    void handleTapGesture(const Leap::Pointable& p);
    bool checkIdle();
    void populatePatternSelector();
    void selectCurrentPattern();
    void incPadAssociation(int padNumber, int inc);
    Leap::Vector scaledLeapInputPosition(const Leap::Vector& v);

    enum TimerIds
    {
        kTimerShowTutorial = 0,
        kTimerLeftHandTap,
        kTimerRightHandTap,
        kTimerCheckLeapConnection,
        kFullscreenTipTimer
    };
    
    class InitGLMessage : public Message {};

    OpenGLContext openGLContext;
    
    MainView* mainView;
    TutorialSlide* tutorial;
    DrumsToolbar* toolbar;
    StatusBar* statusBar;
    std::vector<PlayArea*> playAreas;
    DrumSelector* drumSelector;
    TrigViewBank* trigViewBank;
    ListSelector* kitSelector;
    ListSelector* patternSelector;
    TempoControl* tempoControl;
    ButtonBar* buttonBar;
    HUDView* leapDisconnectedView;
    ScopedPointer<HUDView> fullscreenTipView;
    std::vector<PadView*> pads;
    std::vector<HUDView*> views;
	Slider tempoSlider;

    View2d* splashBgView;
    View2d* splashTitleView;

    typedef std::map<int, StrikeDetector> StrikeDetectorMap;
    StrikeDetectorMap strikeDetectors;
    StrikeDetectorMap toolStrikeDetectors;
    
    float prevMouseY;
    float prevMouseX;
    bool sizeChanged;
    int lastCircleId;
    int64 lastCircleStartTime;
    bool showKitSelector;
    bool showPatternSelector;
    bool isIdle;
    bool needsPatternListUpdate;
	bool setPriority;
    int lastDrumSelection;
    float newCursorW;
    float newCursorH;
    bool resizeCursor;
    
    Image splashBgImage;
    Image splashTitleImage;
    Image splashLogoImage;
    Image splashImage;
    Time lastRender;

    Time lastFrame;
    
    ScopedPointer<StickView> stick1;
    ScopedPointer<StickView> stick2;
    StrikeDetector strikeDetector1;
    StrikeDetector strikeDetector2;
    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_115FADC5__

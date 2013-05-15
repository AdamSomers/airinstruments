/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __MAINCOMPONENT_H_115FADC5__
#define __MAINCOMPONENT_H_115FADC5__

#include "DrumsHUD.h"
#include "PadView.h"
#include "Leap.h"
#include "PlayArea.h"
#include "TrigView.h"
#include "DrumSelector.h"
#include "SkinManager.h"
#include "WheelSelector.h"
#include "TutorialSlide.h"
#include "TempoControl.h"
#include "StrikeDetector.h"
#include "CursorView.h"
#include "ButtonBar.h"

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
                               public WheelSelector::Listener,
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
    
    // KitSelector::Listener override
    void wheelSelectorChanged(WheelSelector* selector);

    // Leap::Listener override
    virtual void onFrame(const Leap::Controller&);
    
    // MultiTimer override
    void timerCallback(int timerId);
    
    // MessageListener override
    void handleMessage(const Message& m);
    
    // ActionListener override
    void actionListenerCallback(const String& message);

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

    enum TimerIds
    {
        kTimerCheckIdle = 0,
        kTimerLeftHandTap,
        kTimerRightHandTap
    };
    
    class InitGLMessage : public Message {};
    
    OpenGLContext openGLContext;
    TutorialSlide* tutorial;
    DrumsToolbar* toolbar;
    StatusBar* statusBar;
    std::vector<PlayArea*> playAreas;
    DrumSelector* drumSelector;
    TrigViewBank* trigViewBank;
    WheelSelector* kitSelector;
    WheelSelector* patternSelector;
    TempoControl* tempoControl;
    ButtonBar* buttonBar;
    std::vector<PadView*> pads;
    std::vector<HUDView*> views;
	Slider tempoSlider;

    View2d* splashView;
    
    typedef std::map<int, StrikeDetector> StrikeDetectorMap;
    StrikeDetectorMap strikeDetectors;
    
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
    Image splashImage;
    Time lastRender;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_115FADC5__

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

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
                               public OpenGLRenderer,
                               public MidiKeyboardStateListener,
                               public Leap::Listener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    // component overrides
    void paint (Graphics&);
    void resized();
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
    
    virtual void onFrame(const Leap::Controller&);
private:
    void layoutPadsGrid();
    void layoutPadsLinear();
    
    void handleTapGesture(const Leap::Pointable& p);
    
    OpenGLContext openGLContext;
    DrumsToolbar* toolbar;
    StatusBar* statusBar;
    PlayArea* playAreaLeft;
    PlayArea* playAreaRight;
    std::vector<PadView*> pads;
    std::vector<HUDView*> views;
    
    float prevMouseY;
    float prevMouseX;
    
    bool sizeChanged;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_115FADC5__

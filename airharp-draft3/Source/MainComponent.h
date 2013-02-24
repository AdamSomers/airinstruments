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

#include "../JuceLibraryCode/JuceHeader.h"



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
                               public OpenGLRenderer,
                               public ChangeListener
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
    bool keyPressed(const KeyPress& kp);
    
    // OpenGLRenderer overrides
    void newOpenGLContextCreated();
    void renderOpenGL();
    void openGLContextClosing();
    
    // ChangeListener override
    void changeListenerCallback (ChangeBroadcaster* source);
    
private:
    void go2d();
    void go3d();
    void setupBackground();
    void layoutStrings();
    void layoutChordRegions();
    bool chordRegionsNeedUpdate = false;
    
    HarpToolbar* toolbar = NULL;
    StatusBar* statusBar = NULL;
    std::vector<ChordRegion*> chordRegions;
    std::vector<HarpView*> harps;
    std::vector<HarpView*> inactiveHarps;
    std::vector<HUDView*> views;
    
    GLuint backgroundTextureId;
    GLBatch backgroundBatch;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // __MAINCOMPONENT_H_115FADC5__

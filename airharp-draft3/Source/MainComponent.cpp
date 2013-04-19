/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#include <GLTools.h>
#include <GLFrustum.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include "HUD.h"

#include "Environment.h"
#include "GfxTools.h"
#include "AudioServer.h"
#include "Harp.h"
#include "MotionServer.h"
#include "FingerView.h"
#include "SkinManager.h"

#include "MainComponent.h"

#include <algorithm>

#define BUFFER_SIZE 512
#define TUTORIAL_TIMEOUT 30000

//RtAudioDriver driver(BUFFER_SIZE);

//==============================================================================
MainContentComponent::MainContentComponent()
{
    Environment::openGLContext.setRenderer (this);
    Environment::openGLContext.setComponentPaintingEnabled (true);
    Environment::openGLContext.attachTo (*this);
    //openGLContext.setSwapInterval(2);
    setSize (800, 600);
    setWantsKeyboardFocus(true);
}

MainContentComponent::~MainContentComponent()
{
    Environment::openGLContext.detach();
    delete toolbar;
    delete statusBar;
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0x00000ff));

    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    // Prevent a divide by zero
    int w = getWidth();
    int h = getHeight();
    if(h == 0)
        h = 1;
    
    // Set Viewport to window dimensions
    //glViewport(0, 0, w, h);
    
    Environment::instance().screenW = w;
    Environment::instance().screenH = h;
    
    sizeChanged = true;
}

void MainContentComponent::focusGained(FocusChangeType cause)
{
    Logger::outputDebugString("Focus Gained");
    MotionDispatcher::instance().resume();
}

void MainContentComponent::focusLost(FocusChangeType cause)
{
    Logger::outputDebugString("Focus Lost");
    MotionDispatcher::instance().pause();
}

void MainContentComponent::setupBackground()
{
    M3DVector3f verts[4] = {
        0, 0, 0.f,
        (float)Environment::instance().screenW, 0.f, 0.f,
        0.f, (float)Environment::instance().screenH, 0.f,
        (float)Environment::instance().screenW, (float)Environment::instance().screenH, 0.f
    };
    
    M3DVector2f texCoords[4] = {
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 0.f
    };
    
    backgroundBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    backgroundBatch.CopyVertexData3f(verts);
    backgroundBatch.CopyTexCoordData2f(texCoords, 0);
    backgroundBatch.End();
}

void MainContentComponent::newOpenGLContextCreated()
{
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);
    //glDepthMask(true);
    //glDepthFunc(GL_LESS);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glEnable(GL_MULTISAMPLE);
    
    //Environment::instance().cameraFrame.MoveForward(-15.0f);

    for (int i = 0; i < HarpManager::instance().getNumHarps(); ++i)
    {
        HarpView* hv = new HarpView(i);
        hv->loadTextures();
        hv->numHarps = HarpManager::instance().getNumHarps();
        harps.push_back(hv);
    }
    
    layoutStrings();
    
    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();

    slide = new TutorialSlide;
    views.push_back(slide);
    slide->begin();
    startTimer(TUTORIAL_TIMEOUT);
    
    HarpToolbar* tb = new HarpToolbar;
    views.push_back(tb);
    toolbar = tb;
    toolbar->updateButtons();
    
    {
    MessageManagerLock mml;
    toolbar->addChangeListener(this);
    }
    
    StatusBar* sb = new StatusBar;
    views.push_back(sb);
    statusBar = sb;
    
    for (int i = 0; i < 7; ++i)
    {
        ChordRegion* cr = new ChordRegion;
        cr->setId(i);
        chordRegions.push_back(cr);
        cr->loadTextures();
    }
    
    for (HUDView* v : views)
        v->loadTextures();
    
    int w = getWidth();
    int h = getHeight();
    toolbar->setBounds(HUDRect(0,h-70,w,70));
    statusBar->setBounds(HUDRect(0,0,w,35));
    
    int yPos = 0;
    int height = h/7;
    for (ChordRegion* cr : chordRegions)
    {
        cr->setBounds(HUDRect(0,yPos, w, height));
        yPos += height;
    }
    
    setupBackground();
    
    SkinManager::instance().getSkin();
    toolbar->setButtonTextures(SkinManager::instance().getSkin().buttonOn, SkinManager::instance().getSkin().buttonOff);
    statusBar->setIndicatorTextures(SkinManager::instance().getSkin().buttonOn, SkinManager::instance().getSkin().buttonOff);
    
    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
    Environment::instance().ready = true;
        
    MotionDispatcher::instance().addListener(*this);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
}

void MainContentComponent::renderOpenGL()
{
    if (sizeChanged)
    {
        if (slide)
            slide->setBounds(HUDRect(Environment::instance().screenW / 2 - 500 / 2,
                                     Environment::instance().screenH / 2 - 225 / 2,
                                     500,
                                     225));
        if (toolbar)
            toolbar->setBounds(HUDRect(0,Environment::instance().screenH-70,Environment::instance().screenW,70));
        if (statusBar)
            statusBar->setBounds(HUDRect(0,0,Environment::instance().screenW,35));
        
        layoutStrings();
        chordRegionsNeedUpdate = true;
        
        if (Environment::instance().ready)
            setupBackground();
    }
    if (chordRegionsNeedUpdate) {
        layoutChordRegions();
        chordRegionsNeedUpdate = false;
    }
    
    // Hack to move a particular slide.  This should be factored into the TutorialSlide class 
    if (slide && slide->getSlideIndex() == 3)
        slide->setBounds(HUDRect(80,
                                 Environment::instance().screenH-80-225,
                                 500,
                                 225));

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    
    glBindTexture(GL_TEXTURE_2D, SkinManager::instance().getSkin().background);
    GLfloat texColor[4] = { 1.f, 1.f, 1.f, 1.f };
    glEnable(GL_BLEND);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), texColor, 0);
    backgroundBatch.Draw();
    
	Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    
    for (HarpView* hv : harps)
        hv->draw();
    
	Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    
    glDisable(GL_DEPTH_TEST);
    
    for (HUDView* v : views)
        v->draw();
    
    glEnable(GL_DEPTH_TEST);
    
    Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();

    for (auto iter : MotionDispatcher::instance().fingerViews)
        if (iter.second->inUse)
            iter.second->draw();
    
    for (auto iter : MotionDispatcher::instance().handViews)
        if (iter.second->inUse)
            ;//iter.second->draw();

    for (HarpView* hv : harps)
        hv->update();
    
    glDisable(GL_DEPTH_TEST);
    
    Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    
    for (ChordRegion* cr : chordRegions)
        cr->draw();
    
    //openGLContext.triggerRepaint();
}

void MainContentComponent::openGLContextClosing()
{
}

void MainContentComponent::layoutStrings()
{
    for (HarpView* hv : harps)
    {
        hv->height = 2.f / harps.size();
        hv->layoutStrings();
    }
}

void MainContentComponent::layoutChordRegions()
{
    if (!Environment::instance().ready)
        return;
    
    Harp* h = HarpManager::instance().getHarp(0);
    if (!h->getChordMode())
        return;
    std::vector<ChordRegion*> activeChordRegions;
    for (ChordRegion* cr : chordRegions)
    {
        if (h->isChordSelected(cr->getId())) {
            activeChordRegions.push_back(cr);
            cr->setActive(true);
        }
        else
            cr->setActive(false);
    }
    int numActiveChords = std::max<int>(1, activeChordRegions.size());
    int height = getHeight() / numActiveChords;
    int yPos = 0;
    for (ChordRegion* cr : activeChordRegions)
    {
        cr->setBounds(HUDRect(0,yPos, getWidth(), height));
        yPos += height;
    }
    
}

void MainContentComponent::mouseMove(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->passiveMotion(e.getPosition().x, e.getPosition().y);
    
    float leapSpaceX = e.getPosition().x / (float)Environment::instance().screenW;
    leapSpaceX *= 400;
    leapSpaceX -= 200;
    float leapSpaceY = 1 - (e.getPosition().y / (float)Environment::instance().screenH);
    leapSpaceY *= 300;
    leapSpaceY += 100;
    //MotionDispatcher::instance().spoof(leapSpaceX, leapSpaceY, -5);
}

void MainContentComponent::mouseDown(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->mouseDown(e.getPosition().x, e.getPosition().y);
}

void MainContentComponent::mouseDrag(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->motion(e.getPosition().x, e.getPosition().y);
}

bool MainContentComponent::keyPressed(const KeyPress& kp)
{
    bool ret = false;
    
    printf("%d\n", kp.getTextDescription().getIntValue());
    if (kp.getTextCharacter() == 'h')
    {
        slide->begin();
        ret = true;
    }
    else if (kp.getTextCharacter() == 'a')
    {
        for (HarpView* hv : harps)
        {
            hv->addString();
        }
        ret = true;
    }
    else if (kp.getTextCharacter() == 'z')
    {
        for (HarpView* hv : harps)
        {
            hv->removeString();
        }
        ret = true;
    }
    else if (kp.getTextCharacter() == 'c')
    {
        Harp* h = HarpManager::instance().getHarp(0);
        h->setChordMode(!h->getChordMode());
        toolbar->updateButtons();
        if (h->getChordMode()) {
            chordRegionsNeedUpdate = true;
        }
        else {
            for (ChordRegion* cr : chordRegions)
                cr->setActive(false);
        }
        
        
        ret = true;
    }
    else if (kp.getTextDescription().getIntValue() > 0)
    {
        SkinManager::instance().setSkinIndex(kp.getTextDescription().getIntValue()-1);
        toolbar->setButtonTextures(SkinManager::instance().getSkin().buttonOn, SkinManager::instance().getSkin().buttonOff);
        statusBar->setIndicatorTextures(SkinManager::instance().getSkin().buttonOn, SkinManager::instance().getSkin().buttonOff);
        ret = true;
    }

    return ret;
}

void MainContentComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    chordRegionsNeedUpdate = true;
}

void MainContentComponent::onFrame(const Leap::Controller& controller)
{
    if (!Environment::instance().ready)
        return;
    
    const Leap::Frame frame = controller.frame();
    const Leap::GestureList& gestures = frame.gestures();
    Leap::GestureList::const_iterator i = gestures.begin();
    Leap::GestureList::const_iterator end = gestures.end();
    for ( ; i != end; ++i)
    {
        const Leap::Gesture& g = *i;
        switch (g.type())
        {
            case Leap::Gesture::TYPE_SWIPE:
                break;
                
            case Leap::Gesture::TYPE_KEY_TAP:
            {
                const Leap::KeyTapGesture keyTap(g);
                handleTapGesture(keyTap.pointable());
            }
                break;
                
            case Leap::Gesture::TYPE_SCREEN_TAP:
            {
                const Leap::ScreenTapGesture screenTap(g);
                handleTapGesture(screenTap.pointable());
            }
                break;
                
            case Leap::Gesture::TYPE_CIRCLE:
            {
                const Leap::CircleGesture circle(g);
                
                bool isClockwise = circle.normal().z < 0;
                
                if (isClockwise && circle.state() == Leap::Gesture::STATE_STOP)
                    slide->end();
                else if (!slide->isDone() && circle.state() == Leap::Gesture::STATE_STOP)
                    slide->back();
            }
                break;
                
            default:
                break;
        }
    }
}

void MainContentComponent::handleTapGesture(const Leap::Pointable &p)
{
    if (!slide->isDone())
        slide->next();
}

void MainContentComponent::timerCallback()
{
    Harp* h = HarpManager::instance().getHarp(0);
    if (h->checkIdle())
        slide->begin();
    else {
        stopTimer();
        startTimer(TUTORIAL_TIMEOUT);
    }
}

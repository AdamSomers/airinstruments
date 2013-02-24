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
    
    if (toolbar)
        toolbar->setBounds(HUDRect(0,h-70,w,70));
    if (statusBar)
        statusBar->setBounds(HUDRect(0,0,w,35));
    
    layoutStrings();
    chordRegionsNeedUpdate = true;
    
    if (Environment::instance().ready)
        setupBackground();
    
    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);    
    Environment::instance().ready = true;
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
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
}

void MainContentComponent::renderOpenGL()
{
    if (chordRegionsNeedUpdate) {
        layoutChordRegions();
        chordRegionsNeedUpdate = false;
    }

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
    MotionDispatcher::instance().spoof(leapSpaceX, leapSpaceY, -5);
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

    if (kp.getTextCharacter() == 'a')
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

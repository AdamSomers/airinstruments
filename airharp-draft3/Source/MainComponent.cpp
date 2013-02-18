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

#include "MainComponent.h"

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
        toolbar->setBounds(HUDRect(0,h-50,w,50));
    if (statusBar)
        statusBar->setBounds(HUDRect(0,0,w,20));
    
    layoutStrings();
    
    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
	Environment::instance().viewFrustum.SetPerspective(10.0f, float(w)/float(h), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
	Environment::instance().modelViewMatrix.LoadIdentity();
    
    Environment::instance().ready = true;
}

void MainContentComponent::newOpenGLContextCreated()
{
    //glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    
    glEnable(GL_DEPTH_TEST);
    //glDepthMask(true);
    //glDepthFunc(GL_LESS);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    //Environment::instance().cameraFrame.MoveForward(-15.0f);

    for (int i = 0; i < HarpManager::instance().getNumHarps(); ++i)
    {
        HarpView* hv = new HarpView(i);
        hv->numHarps = HarpManager::instance().getNumHarps();
        harps.push_back(hv);
    }
    
    layoutStrings();
    
    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();

    HarpToolbar* tb = new HarpToolbar;
    views.push_back(tb);
    toolbar = tb;
    
    StatusBar* sb = new StatusBar;
    views.push_back(sb);
    statusBar = sb;
    
    for (HUDView* v : views)
        v->loadTextures();
    
    int w = getWidth();
    int h = getHeight();
    toolbar->setBounds(HUDRect(0,h-50,w,50));
    statusBar->setBounds(HUDRect(0,0,w,20));

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
}

void MainContentComponent::renderOpenGL()
{
    //OpenGLShaderProgram shaderProgram(Environment::openGLContext);
    //shaderProgram.addShader(BinaryData::testShader_vs, GL_VERTEX_SHADER);
    //shaderProgram.addShader(BinaryData::testShader_fs, GL_FRAGMENT_SHADER);
    //shaderProgram.link();
    //shaderProgram.use();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    
    //shaderProgram.use();
    
    for (auto iter : MotionDispatcher::instance().fingerViews)
        if (iter.second->inUse)
            iter.second->draw();
    
    for (auto iter : MotionDispatcher::instance().handViews)
        if (iter.second->inUse)
            ;//iter.second->draw();
    
    for (HarpView* hv : harps)
        hv->update();
    
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

void MainContentComponent::mouseMove(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->passiveMotion(e.getPosition().x, e.getPosition().y);
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
        HarpManager::instance().getHarp(0)->setChordMode(!HarpManager::instance().getHarp(0)->getChordMode());
        
        ret = true;
    }


    return ret;
}

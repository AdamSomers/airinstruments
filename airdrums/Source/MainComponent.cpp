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
#include "Drums.h"
#include "MotionServer.h"
#include "FingerView.h"

#include "MainComponent.h"

#define NUM_PADS 16

//==============================================================================
MainContentComponent::MainContentComponent()
{
    openGLContext.setRenderer (this);
    openGLContext.setComponentPaintingEnabled (true);
    openGLContext.attachTo (*this);
    openGLContext.setSwapInterval(1);
    setSize (800, 600);
}

MainContentComponent::~MainContentComponent()
{
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
    
    layoutPads();
    
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

    for (int i = 0; i < NUM_PADS; ++i)
    {
        PadView* pv = new PadView;
        pv->setup();
        pv->padNum = i;
        pads.push_back(pv);
    }
    
    layoutPads();
    
    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();

    DrumsToolbar* tb = new DrumsToolbar;
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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    
//    Environment::instance().modelViewMatrix.PushMatrix();
//    M3DMatrix44f mCamera;
//    Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
//    Environment::instance().modelViewMatrix.MultMatrix(mCamera);
    Environment::instance().modelViewMatrix.PushMatrix(padSurfaceFrame);
    
    for (PadView* pv : pads)
        pv->draw();
    
    Environment::instance().modelViewMatrix.PopMatrix();
//    Environment::instance().modelViewMatrix.PopMatrix();
    
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
    
    for (PadView* pv : pads)
        pv->update();
    
    //openGLContext.triggerRepaint();
}

void MainContentComponent::openGLContextClosing()
{
}

void MainContentComponent::layoutPads()
{
    if (pads.size() == 0)
        return;
    float aspectRatio = Environment::screenW / (float)Environment::screenH;
    float left = -1.f;
    float top = .8;
    float right = 1.f;
    float bottom = -1.f;
    float width = right - left;
    float height = top - bottom;
    float padWidth = width / 4.f;
    float padHeight = height / 4.f;
    float initialX = left + padWidth;
    float initialY = top;
    for (int i = 0; i < 16; ++i)
    {
        int padInRow = i % 4;
        int row = i / 4;
        float xpos = padWidth * padInRow + initialX;
        float ypos = -padHeight * row + initialY;
        pads.at(i)->objectFrame.SetOrigin(xpos, ypos, 0);
        pads.at(i)->padWidth = padWidth-0.01;
        pads.at(i)->padHeight = padHeight-0.01;
        pads.at(i)->update();
//        pads.at(i)->objectFrame.RotateWorld(m3dDegToRad(15), 1, 0, 0);
    }
    padSurfaceFrame.SetOrigin(0,0,-12);
    padSurfaceFrame.SetForwardVector(0, 0, 1);
    padSurfaceFrame.RotateWorld(m3dDegToRad(-45), 1, 0, 0);
    //Environment::instance().cameraFrame.TranslateWorld(0, 0, -2);
}

void MainContentComponent::mouseMove(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->passiveMotion(e.getPosition().x, e.getPosition().y);
    
    float normPos = e.getPosition().y / (float)Environment::instance().screenH;
    float angle = M_PI * 2 * normPos;
    printf("%f %f\n", sin(angle), cos(angle));
    if (pads.size() == 0)
        return;
    padSurfaceFrame.RotateWorld(m3dDegToRad(-.5), 1, 0, 0);
    for (int i = 0; i < 16; ++i)
    {
        //pads.at(i)->objectFrame.RotateWorld(m3dDegToRad(-0.5), 1, 0, 0);
    }
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

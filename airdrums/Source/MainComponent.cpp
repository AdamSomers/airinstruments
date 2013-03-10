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
    MotionDispatcher::zLimit = -100;
    Drums::instance().playbackState.addListener(this);
    setWantsKeyboardFocus(true);
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
    
    layoutPadsLinear();
    
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
    glDepthMask(true);
    glDepthFunc(GL_LESS);
    
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
    
    layoutPadsLinear();
    Environment::instance().cameraFrame.TranslateWorld(0, -.75, 0);
    Environment::instance().cameraFrame.TranslateWorld(6, 0, 0);
    PadView::padSurfaceFrame.RotateWorld(m3dDegToRad(-50), 1, 0, 0);
    
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

    MotionDispatcher::instance().controller.addListener(*this);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
    
    glClearColor(0.f, 0.f, 0.f, 1.0f );
}

void MainContentComponent::renderOpenGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();
    
    //glEnable(GL_DEPTH_TEST);
    

    Environment::instance().modelViewMatrix.PushMatrix(PadView::padSurfaceFrame);
    
    Environment::instance().modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    Environment::instance().cameraFrame.GetCameraMatrix(mCamera);
    Environment::instance().modelViewMatrix.MultMatrix(mCamera);


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Objects must be drawn from front to back to ensure proper visibility.
    std::vector<PadView*> sortedPads;
    for (PadView* pv : pads)
        sortedPads.push_back(pv);
    std::sort(sortedPads.begin(),
              sortedPads.end(),
              [](PadView* p1, PadView* p2) ->
                  bool
                  {
                      // compare z values post pad-plane transform
                      M3DVector3f p1center;
                      M3DVector3f p2center;
                      p1->objectFrame.GetOrigin(p1center);
                      p2->objectFrame.GetOrigin(p2center);
                      M3DVector3f transformedCenter1;
                      M3DVector3f transformedCenter2;
                      M3DMatrix44f planeMatrix;
                      PadView::padSurfaceFrame.GetMatrix(planeMatrix);
                      m3dTransformVector3(transformedCenter1, p1center, planeMatrix);
                      m3dTransformVector3(transformedCenter2, p2center, planeMatrix);
                      return (transformedCenter1[2] < transformedCenter2[2]);
                  }
              );

    // We sorted back-to-front, so draw in reverse for front-to-back
    for (int i = sortedPads.size() - 1; i >= 0; --i)
        sortedPads[i]->draw();

    Environment::instance().modelViewMatrix.PopMatrix(); // pad plane

    Environment::instance().modelViewMatrix.PopMatrix(); // camera
    
    // go 2d
	Environment::instance().viewFrustum.SetOrthographic(0, Environment::instance().screenW, 0.0f, Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    
    glDisable(GL_DEPTH_TEST);
    
    for (HUDView* v : views)
        v->draw();
    
    glEnable(GL_DEPTH_TEST);

    // go 3d
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

void MainContentComponent::layoutPadsGrid()
{
    if (pads.size() == 0)
        return;
    float aspectRatio = Environment::screenW / (float)Environment::screenH;
    float left = -1.f;
    float top = .8;
    float right = 1.f;
    float bottom = -1.2f;
    float width = right - left;
    float height = top - bottom - .2;
    float padWidth = width / 4.f;
    float padHeight = height / 4.f;
    float initialX = left + padWidth / 2.f;
    float initialY = top;
    for (int i = 0; i < NUM_PADS; ++i)
    {
        int padInRow = i % 4;
        int row = i / 4;
        float xpos = padWidth * padInRow + initialX;
        float ypos = -padHeight * row + initialY;
        pads.at(i)->objectFrame.SetOrigin(xpos, ypos, 0);
        pads.at(i)->padWidth = padWidth-0.05;
        pads.at(i)->padHeight = padHeight-0.05;
        pads.at(i)->update();
    }
    // Move the pads back -12
    PadView::padSurfaceFrame.SetOrigin(0,0,-12);
}

void MainContentComponent::layoutPadsLinear()
{
    if (pads.size() == 0)
        return;
    float aspectRatio = Environment::screenW / (float)Environment::screenH;
    float left = -1.f;
    float top = .8;
    float right = 1.f;
    float bottom = -1.2f;
    float width = right - left;
    float height = top - bottom - .2;
    float padWidth = width / 4.f;
    float padHeight = height;
    float initialX = left + padWidth / 2.f;
    float initialY = top;
    for (int i = 0; i < NUM_PADS; ++i)
    {
        int padInRow = i;
        float xpos = padWidth * padInRow + initialX;
        float ypos = -padHeight + initialY;
        pads.at(i)->objectFrame.SetOrigin(xpos, ypos, 0);
        pads.at(i)->padWidth = padWidth-0.05;
        pads.at(i)->padHeight = padHeight-0.05;
        pads.at(i)->update();
    }
    // Move the pads back -12
    PadView::padSurfaceFrame.SetOrigin(0,0,-12);
}

void MainContentComponent::mouseMove(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->passiveMotion(e.getPosition().x, e.getPosition().y);
    
    if (pads.size() == 0)
        return;
}

void MainContentComponent::mouseDown(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->mouseDown(e.getPosition().x, e.getPosition().y);
    
    prevMouseY = e.getPosition().y;
    prevMouseX = e.getPosition().x;
}

void MainContentComponent::mouseDrag(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->motion(e.getPosition().x, e.getPosition().y);
    
    float normPosY = (e.getPosition().y - prevMouseY) / (float)Environment::instance().screenH;
    float normPosX = (e.getPosition().x - prevMouseX) / (float)Environment::instance().screenW;
    
    Environment::instance().cameraFrame.TranslateWorld(normPosX*2, 0, 0);
    Environment::instance().cameraFrame.TranslateWorld(0, 0,-normPosY*2);
    
//    float angle = M_PI * 2 * normPosY;
//    PadView::padSurfaceFrame.RotateWorld(m3dDegToRad(normPosY*720), 1, 0, 0);
//    angle = M_PI * 2 * normPosX;
//    PadView::padSurfaceFrame.RotateWorld(m3dDegToRad(normPosX*720), 0, 1, 0);
//    for (int i = 0; i < 16; ++i)
//    {
//        //pads.at(i)->objectFrame.RotateWorld(m3dDegToRad(-0.5), 1, 0, 0);
//    }
    
    prevMouseY = e.getPosition().y;
    prevMouseX = e.getPosition().x;
}

void MainContentComponent::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    Environment::instance().cameraFrame.TranslateWorld(0, wheel.deltaY*4,wheel.deltaY*4);
}

bool MainContentComponent::keyPressed(const KeyPress& kp)
{
    bool ret = false;
    if (kp.getTextCharacter() == 'm') {
        Drums::instance().metronomeOn = !Drums::instance().metronomeOn;
        ret = true;
    }
    else if (kp.getKeyCode() == KeyPress::spaceKey) {
        Drums::instance().recording = !Drums::instance().recording;
        ret = true;
    }
    if (kp.getTextCharacter() == 'c') {
        Drums::instance().clear();
        ret = true;
    }
    return ret;
}

void MainContentComponent::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    if (midiNoteNumber < pads.size())
    {
        pads.at(midiNoteNumber)->triggerDisplay();
    }
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
            {
                Leap::SwipeGesture swipe(g);
                Environment::instance().cameraFrame.TranslateWorld((swipe.direction().x * swipe.speed()) * .00002, 0, 0);
                
            }
                break;
                
            default:
                break;
        }
    }
}

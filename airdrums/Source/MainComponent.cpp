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
MainContentComponent::MainContentComponent() :
  toolbar(NULL)
, statusBar(NULL)
, prevMouseY(0.f)
, prevMouseX(0.f)
, sizeChanged(false)
, playAreaLeft(NULL)
, playAreaRight(NULL)
, trigViewBank(NULL)
{
    openGLContext.setRenderer (this);
    openGLContext.setComponentPaintingEnabled (true);
    openGLContext.attachTo (*this);
    openGLContext.setSwapInterval(1);
    setSize (800, 600);
    MotionDispatcher::zLimit = -100;
    Drums::instance().playbackState.addListener(this);
    setWantsKeyboardFocus(true);
    
    PropertiesFile::Options options;
    options.applicationName = "AirBeats";
    options.filenameSuffix = ".settings";
    options.folderName = "AirBeats";
    options.osxLibrarySubFolder = "Application Support";
    options.commonToAllUsers = "false";
    options.ignoreCaseOfKeyNames = true;
    options.millisecondsBeforeSaving = 0;
    options.storageFormat = PropertiesFile::storeAsXML;
    properties.setStorageParameters(options);
}

MainContentComponent::~MainContentComponent()
{
	openGLContext.detach();
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0x00000ff));

    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    //g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
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

#if 0
    for (int i = 0; i < NUM_PADS; ++i)
    {
        PadView* pv = new PadView;
        pv->setup();
        pv->padNum = i;
        pads.push_back(pv);
    }
    layoutPadsLinear();
#endif
    
    Environment::instance().cameraFrame.TranslateWorld(0, -.75, 0);
    Environment::instance().cameraFrame.TranslateWorld(6, 0, 0);
    PadView::padSurfaceFrame.RotateWorld((float) m3dDegToRad(-50), 1, 0, 0);

#ifdef _WIN32
	glewInit();		// Not sure if this is in the right place, but it seems to work for now.
#endif // _WIN32

    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();

    DrumsToolbar* tb = new DrumsToolbar;
    views.push_back(tb);
    toolbar = tb;
    
    StatusBar* sb = new StatusBar;
    views.push_back(sb);
    statusBar = sb;
    
    playAreaLeft = new PlayArea;
    int note = properties.getUserSettings()->getIntValue("selectedNoteLeft", 13);
    playAreaLeft->setSelectedMidiNote(note);
    playAreaRight = new PlayArea;
    note = properties.getUserSettings()->getIntValue("selectedNoteRight", 12);
    playAreaRight->setSelectedMidiNote(note);
    views.push_back(playAreaLeft);
    views.push_back(playAreaRight);
    trigViewBank = new TrigViewBank;
    views.push_back(trigViewBank);
    
    for (HUDView* v : views)
        v->loadTextures();
    
    int w = getWidth();
    int h = getHeight();
    toolbar->setBounds(HUDRect(0,(GLfloat) h-50,(GLfloat) w,50));
    statusBar->setBounds(HUDRect(0,0,(GLfloat) w,20));

    MotionDispatcher::instance().controller.addListener(*this);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
    
    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
    
    glClearColor(0.f, 0.f, 0.f, 1.0f );
}

void MainContentComponent::renderOpenGL()
{
    if (sizeChanged)
    {
        const int toobarHeight = 50;
        const int statusBarHeight = 20;
        const int playAreaHeight = Environment::instance().screenH - toobarHeight - statusBarHeight - 10;
        const int playAreaWidth = Environment::instance().screenW / 2 - 10;
        
        if (toolbar)
            toolbar->setBounds(HUDRect(0,
                                       (GLfloat) Environment::instance().screenH-toobarHeight,
                                       (GLfloat) Environment::instance().screenW,
                                       toobarHeight));
        
        if (statusBar)
            statusBar->setBounds(HUDRect(0,
                                         0,
                                         (GLfloat) Environment::instance().screenW,
                                         statusBarHeight));
        
        if (playAreaLeft)
            playAreaLeft->setBounds(HUDRect(5,
                                            (GLfloat) statusBarHeight + 5,
                                            (GLfloat) playAreaWidth,
                                            playAreaHeight));
                                    
        if (playAreaRight)
            playAreaRight->setBounds(HUDRect(Environment::instance().screenW / 2 + 5,
                                            (GLfloat) statusBarHeight + 5,
                                            (GLfloat) playAreaWidth,
                                            playAreaHeight));
        if (trigViewBank)
            trigViewBank->setBounds(HUDRect(0,
                                            (GLfloat) Environment::instance().screenH-toobarHeight,
                                            (GLfloat) Environment::instance().screenW / 4,
                                            toobarHeight));
        
        layoutPadsLinear();
        sizeChanged = false;
    }
    
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

	Environment::instance().viewFrustum.SetPerspective(10.0f, float(Environment::instance().screenW)/float(Environment::instance().screenH), 0.01f, 500.0f);
	Environment::instance().projectionMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());
    Environment::instance().modelViewMatrix.LoadIdentity();

#if 0
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
#endif
    
    // go 2d
	Environment::instance().viewFrustum.SetOrthographic(0, (GLfloat) Environment::instance().screenW, 0.0f, (GLfloat) Environment::instance().screenH, 800.0f, -800.0f);
	Environment::instance().modelViewMatrix.LoadMatrix(Environment::instance().viewFrustum.GetProjectionMatrix());

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
}

void MainContentComponent::openGLContextClosing()
{
}

void MainContentComponent::layoutPadsGrid()
{
    if (pads.size() == 0)
        return;
    //float aspectRatio = Environment::screenW / (float)Environment::screenH;	// Unused variable
    float left = -1.f;
    float top = .8f;
    float right = 1.f;
    float bottom = -1.2f;
    float width = right - left;
    float height = top - bottom - .2f;
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
        pads.at(i)->padWidth = padWidth-0.05f;
        pads.at(i)->padHeight = padHeight-0.05f;
        pads.at(i)->update();
    }
    // Move the pads back -12
    PadView::padSurfaceFrame.SetOrigin(0,0,-12);
}

void MainContentComponent::layoutPadsLinear()
{
    if (pads.size() == 0)
        return;
    //float aspectRatio = Environment::screenW / (float)Environment::screenH;	// Unused variable
    float left = -1.f;
    float top = .8f;
    float right = 1.f;
    float bottom = -1.2f;
    float width = right - left;
    float height = top - bottom - .2f;
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
        pads.at(i)->padWidth = padWidth-0.05f;
        pads.at(i)->padHeight = padHeight-0.05f;
        pads.at(i)->update();
    }
    // Move the pads back -12
    PadView::padSurfaceFrame.SetOrigin(0,0,-12);
}

void MainContentComponent::mouseMove(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->passiveMotion((float) e.getPosition().x, (float) e.getPosition().y);
    
    if (pads.size() == 0)
        return;
}

void MainContentComponent::mouseDown(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->mouseDown((float) e.getPosition().x, (float) e.getPosition().y);
    
    prevMouseY = (float) e.getPosition().y;
    prevMouseX = (float) e.getPosition().x;
    
    if (e.getPosition().x < Environment::instance().screenW / 2)
    {
        Drums::instance().NoteOn(playAreaLeft->getSelectedMidiNote(), 1.f);
        //playAreaLeft->tap(playAreaLeft->getSelectedMidiNote());
    }
    else
    {
        Drums::instance().NoteOn(playAreaRight->getSelectedMidiNote(), 1.f);
        //playAreaRight->tap(playAreaRight->getSelectedMidiNote());
    }
}

void MainContentComponent::mouseDrag(const MouseEvent& e)
{
    for (HUDView* v : views)
        v->motion((float) e.getPosition().x, (float) e.getPosition().y);
    
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
    
    prevMouseY = (float) e.getPosition().y;
    prevMouseX = (float) e.getPosition().x;
}

void MainContentComponent::mouseWheelMove (const MouseEvent& /*e*/, const MouseWheelDetails& wheel)
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
    else if (kp.getTextCharacter() == 'c') {
        Drums::instance().clear();
        ret = true;
    }
    else if (kp.getTextCharacter() == 'q') {
        playAreaLeft->setSelectedMidiNote(playAreaLeft->getSelectedMidiNote() - 1);
        properties.getUserSettings()->setValue("selectedNoteLeft", playAreaLeft->getSelectedMidiNote());
    }
    else if (kp.getTextCharacter() == 'w') {
        playAreaLeft->setSelectedMidiNote(playAreaLeft->getSelectedMidiNote() + 1);
        properties.getUserSettings()->setValue("selectedNoteLeft", playAreaLeft->getSelectedMidiNote());
    }
    else if (kp.getTextCharacter() == 'a') {
        playAreaRight->setSelectedMidiNote(playAreaRight->getSelectedMidiNote() - 1);
        properties.getUserSettings()->setValue("selectedNoteRight", playAreaRight->getSelectedMidiNote());
    }
    else if (kp.getTextCharacter() == 's') {
        playAreaRight->setSelectedMidiNote(playAreaRight->getSelectedMidiNote() + 1);
        properties.getUserSettings()->setValue("selectedNoteRight", playAreaRight->getSelectedMidiNote());
    }
    
    properties.saveIfNeeded();
    return ret;
}

void MainContentComponent::handleNoteOn(MidiKeyboardState* /*source*/, int /*midiChannel*/, int midiNoteNumber, float /*velocity*/)
{
    if ((unsigned int) midiNoteNumber < pads.size())
    {
        pads.at(midiNoteNumber)->triggerDisplay();
    }

    playAreaLeft->tap(midiNoteNumber);
    playAreaRight->tap(midiNoteNumber);
    
    trigViewBank->trigger(midiNoteNumber);
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
                Environment::instance().cameraFrame.TranslateWorld((swipe.direction().x * swipe.speed()) * .00002f, 0, 0);
                
            }
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
                
            default:
                break;
        }
    }
}

void MainContentComponent::handleTapGesture(const Leap::Pointable &p)
{
    if (p.tipPosition().x < 0)
    {
        Drums::instance().NoteOn(playAreaLeft->getSelectedMidiNote(), 1.f);
        //playAreaLeft->tap(playAreaLeft->getSelectedMidiNote());
    }
    else
    {
        Drums::instance().NoteOn(playAreaRight->getSelectedMidiNote(), 1.f);
        //playAreaRight->tap(playAreaRight->getSelectedMidiNote());
    }
}

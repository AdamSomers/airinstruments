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

#include "Main.h"
#include "Environment.h"
#include "GfxTools.h"
#include "Drums.h"
#include "MotionServer.h"
#include "FingerView.h"
#include "KitManager.h"
#include "PatternManager.h"

#include "MainComponent.h"

#define NUM_PADS 16
#define TUTORIAL_TIMEOUT 30000
#define TAP_TIMEOUT 50
#define SPLASH_FADE 1500

//==============================================================================
MainContentComponent::MainContentComponent()
: tutorial(NULL)
, toolbar(NULL)
, statusBar(NULL)
, prevMouseY(0.f)
, prevMouseX(0.f)
, sizeChanged(false)
, drumSelector(NULL) 
, trigViewBank(NULL)
, kitSelector(NULL)
, patternSelector(NULL)
, tempoControl(NULL)
, buttonBar(NULL)
, splashBgView(NULL)
, splashTitleView(NULL)
, lastCircleId(0)
, showKitSelector(false)
, tempoSlider(Slider::LinearHorizontal, Slider:: NoTextBox)
, isIdle(true)
, needsPatternListUpdate(false)
, setPriority(false)
, lastDrumSelection(-1)
, resizeCursor(false)
{
    setSize (1200, 750);
    MotionDispatcher::zLimit = -100;
    setWantsKeyboardFocus(true);

	tempoSlider.setRange(30.0, 300.0, 0.1);
	tempoSlider.setSize(250, 50);
	addAndMakeVisible(&tempoSlider);
	tempoSlider.setCentrePosition(600, 30);
	AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	float tempo = (float) props.getUserSettings()->getDoubleValue("tempo", (double) DrumPattern::kDefaultTempo);
	tempoSlider.setValue(tempo);
    tempoSlider.setVisible(false);
    
    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File resourcesFile = special.getChildFile("../");
#elif JUCE_MAC
	File resourcesFile = special.getChildFile("Contents/Resources");
#endif
    File bgImageFile = resourcesFile.getChildFile("splash_bg.png");
    File splashTitleImageFile = resourcesFile.getChildFile("splash_title.png");
    
    if (bgImageFile.exists())
        splashBgImage = ImageFileFormat::loadFrom(bgImageFile);
    else
        Logger::outputDebugString("ERROR: splash_bg.png not found!");
    
    if (splashTitleImageFile.exists())
        splashTitleImage = ImageFileFormat::loadFrom(splashTitleImageFile);
    else
        Logger::outputDebugString("ERROR: splash_title.png not found!");
}

MainContentComponent::~MainContentComponent()
{
//    MotionDispatcher::instance().removeAllListeners();
    MotionDispatcher::instance().removeListener(*this);
	openGLContext.detach();
//    openGLContext.deactivateCurrentContext();
    views.clear();
    delete tutorial;
    delete toolbar;
    delete statusBar;
    delete drumSelector;
    delete trigViewBank;
    delete kitSelector;
    delete patternSelector;
    delete tempoControl;
    delete buttonBar;
    delete splashBgView;
    delete splashTitleView;
    
    for (PlayArea* pad : playAreas)
        delete pad;
}

void MainContentComponent::paint (Graphics& g)
{
    static bool firstTime = true;
    if (Environment::instance().ready)
        return;

    splashImage = Image(Image::ARGB, getWidth(), getHeight(), true);
    Graphics offscreen(splashImage);
    g.fillAll (Colour (0x00000ff));
    g.setFont (Font (16.0f));
    g.setColour (Colours::black);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
    if (splashTitleImage.isValid()) {
        float aspectRatio = splashTitleImage.getHeight() / (float)splashTitleImage.getWidth();
        float w = getWidth() / 2.f;
        float h = w * aspectRatio;
        int x = (int)(getWidth() / 2.f - w / 2.f);
        int y = (int)(getHeight() / 2.f - h / 2.f);
        offscreen.drawImage(splashTitleImage, x, y, (int)w, (int)h, 0, 0, splashTitleImage.getWidth(), splashTitleImage.getHeight());
        offscreen.setColour (Colour (0xffffffff));
        Font f = Font(Environment::instance().getDefaultFont(), 16, Font::plain);
        f.setExtraKerningFactor(1.5);
        offscreen.setFont(f);
        offscreen.drawText("LOADING", x, y + (int)h + 20, (int)w, 12, Justification::centred, false);
    }
    if (splashBgImage.isValid())
        g.drawImage(splashBgImage, 0, 0, getWidth(), getHeight(), 0, 0, splashBgImage.getWidth(), splashBgImage.getHeight());
    g.drawImage(splashImage, 0, 0, getWidth(), getHeight(), 0, 0, splashImage.getWidth(), splashImage.getHeight());
    
    if (firstTime) {
        Logger::outputDebugString("painted once, sending InitGLMessage");
        postMessage(new InitGLMessage);
    }
    firstTime = false;
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

void MainContentComponent::focusGained(FocusChangeType /*cause*/)
{
    Logger::outputDebugString("Focus Gained");
    MotionDispatcher::instance().resume();
}

void MainContentComponent::focusLost(FocusChangeType /*cause*/)
{
    Logger::outputDebugString("Focus Lost");
    MotionDispatcher::instance().pause();
}

void MainContentComponent::newOpenGLContextCreated()
{
    Logger::outputDebugString("newOpenGLContextCreated()");
    
    Drums::instance().playbackState.addListener(this);
    Drums::instance().registerTempoSlider(&tempoSlider);
    tempoSlider.addListener(&Drums::instance());

#ifdef _WIN32
	glewInit();		// Not sure if this is in the right place, but it seems to work for now.
#endif // _WIN32

    SkinManager::instance().loadResources();
    KitManager::GetInstance().LoadTextures();
    //String skinSetting = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("skin", "Default");
    //SkinManager::instance().setSelectedSkin(skinSetting);
    
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

    glEnable(GL_DEPTH_TEST);
    Environment::instance().shaderManager.InitializeStockShaders();

    DrumsToolbar* tb = new DrumsToolbar;
    views.push_back(tb);
    toolbar = tb;

    StatusBar* sb = new StatusBar;
    views.push_back(sb);
    statusBar = sb;
    
    int layout = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("layout", StrikeDetector::kLayout3x2);
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("layout", layout);
    
    drumSelector = new DrumSelector;
    drumSelector->addActionListener(this);
    views.push_back(drumSelector);
    
    const String defaultPadColors[6] = { "ff8080ff", "ffff8080", "ff080ff80", "ff80ffff", "ffff80ff", "ffffff80" };
    
    for (int i = 0; i < 6; ++i)
    {
        PlayArea* pad = new PlayArea(i);
        int midiNote = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedNote" + String(i), i);
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("selectedNote" + String(i), midiNote);
        pad->setSelectedMidiNote(midiNote);
        
        String color = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("padColor" + String(i), defaultPadColors[i]);
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("padColor" + String(i), color);
        
        pad->setColor(Colour::fromString(color));

        playAreas.push_back(pad);
        views.push_back(pad);
        pad->addActionListener(this);
        
        drumSelector->setPadAssociation(midiNote, i);
    }

    trigViewBank = new TrigViewBank;
    views.push_back(trigViewBank);
    
    kitSelector = new WheelSelector(true);
    int numKits = KitManager::GetInstance().GetItemCount();
    for (int i = 0; i < numKits; ++i)
    {
        WheelSelector::Icon* icon = new WheelSelector::Icon(i, true);
        icon->setDefaultTexture(KitManager::GetInstance().GetItem(i)->GetTexture());
        kitSelector->addIcon(icon);
    }

    kitSelector->addListener(this);
    views.push_back(kitSelector);
    
    patternSelector = new WheelSelector;
    populatePatternSelector();
    
    patternSelector->addListener(this);
    views.push_back(patternSelector);
    
    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid");
    Uuid kitUuid(kitUuidString);
    String kitName = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitName");
    Logger::outputDebugString("Selected kit: " + kitUuidString);
    Logger::outputDebugString("Selected kit: " + kitName);
    int selectedKitIndex = 0;
    for (int i = 0; i < numKits; ++i)
    {
        std::shared_ptr<DrumKit> kit = KitManager::GetInstance().GetItem(i);
        if (kit->GetName() == kitName) {
            selectedKitIndex = i;
            break;
        }
    }
    Logger::outputDebugString("Selected kit: " + String(selectedKitIndex));
    kitSelector->setSelection(selectedKitIndex);
    Drums::instance().setDrumKit(KitManager::GetInstance().GetItem(selectedKitIndex));
    
    tutorial = new TutorialSlide;
    tutorial->loadTextures();
    tutorial->setButtonRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    tutorial->addActionListener(this);
    tutorial->setVisible(false, 0);
    startTimer(kTimerShowTutorial, 500);

    tempoControl = new TempoControl;
    float tempo = (float) AirHarpApplication::getInstance()->getProperties().getUserSettings()->getDoubleValue("tempo", (double) DrumPattern::kDefaultTempo);
    tempoControl->setTempo(tempo);
    views.push_back(tempoControl);
    
    buttonBar = new ButtonBar;
    buttonBar->addActionListener(this);
    GLfloat transparent[4] = { 0.f, 0.f, 0.f, 0.f };
    buttonBar->setDefaultColor(transparent);
    views.push_back(buttonBar);

    for (HUDView* v : views) {
        v->loadTextures();
        v->setVisible(false);
    }

    int w = getWidth();
    int h = getHeight();
    toolbar->setBounds(HUDRect(0,(GLfloat) h-50,(GLfloat) w,50));
    statusBar->setBounds(HUDRect(0,0,(GLfloat) w,20));
    
    splashBgView = new View2d;
    splashBgView->setBounds(HUDRect(0,0,(GLfloat)w,(GLfloat)h));
    splashBgView->setDefaultTexture(GfxTools::loadTextureFromJuceImage(splashBgImage));
    splashTitleView = new View2d;
    splashTitleView->setBounds(HUDRect(0,0,(GLfloat)w,(GLfloat)h));
    splashTitleView->setDefaultTexture(GfxTools::loadTextureFromJuceImage(splashImage));
    splashTitleView->setVisible(false, SPLASH_FADE);

    MotionDispatcher::instance().addListener(*this);
    
#if 0 // disabled all gestures in favor of buttons
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
    MotionDispatcher::instance().controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
#endif

    Environment::instance().transformPipeline.SetMatrixStacks(Environment::instance().modelViewMatrix, Environment::instance().projectionMatrix);
    
    MotionDispatcher::instance().setCursorTexture(SkinManager::instance().getSelectedSkin().getTexture("cursor"));

    glClearColor(0.f, 0.f, 0.f, 1.0f );

    openGLContext.setSwapInterval(1);

    Environment::instance().ready = true;
}

void MainContentComponent::populatePatternSelector()
{
    patternSelector->removeAllIcons();
    
    int numPatterns = PatternManager::GetInstance().GetItemCount();
    if (numPatterns == 0)
    {
        Logger::outputDebugString("no patterns!");
        return;
    }
    for (int i = 0; i < numPatterns; ++i)
    {
        WheelSelector::Icon* icon = new WheelSelector::Icon(i);
        Image im(Image::PixelFormat::ARGB, 2000, 200, true);
        Graphics g (im);
        g.setColour(Colour::fromRGBA(60, 60, 60, 255));
        g.setFont(Font(Environment::instance().getDefaultFont(), 200, Font::plain));
        g.drawText(PatternManager::GetInstance().GetItem(i)->GetName(), 0, 0, 1500, 200, Justification::left, true);

        TextureDescription textureDesc = GfxTools::loadTextureFromJuceImage(im, true);
        textureDesc.imageW = im.getWidth();
        textureDesc.imageH = im.getHeight();
        icon->setDefaultTexture(textureDesc);
        
        patternSelector->addIcon(icon);
    }
    selectCurrentPattern();
}

void MainContentComponent::selectCurrentPattern()
{
    String patternUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("patternUuid");
    Uuid patternUuid(patternUuidString);
    String patternName = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("patternName");
    Logger::outputDebugString("Selected pattern: " + patternUuidString);
    Logger::outputDebugString("Selected pattern: " + patternName);
    int selectedpatternIndex = 0;
    int numPatterns = PatternManager::GetInstance().GetItemCount();
    for (int i = 0; i < numPatterns; ++i)
    {
        std::shared_ptr<DrumPattern> pattern = PatternManager::GetInstance().GetItem(i);
        if (pattern->GetName() == patternName) {
            selectedpatternIndex = i;
            break;
        }
    }
    Logger::outputDebugString("Selected pattern: " + String(selectedpatternIndex));
    patternSelector->setSelection(selectedpatternIndex);
    Drums::instance().setPattern(PatternManager::GetInstance().GetItem(selectedpatternIndex));
}

void MainContentComponent::renderOpenGL()
{
    if (resizeCursor)
    {
        MotionDispatcher::instance().cursor->setBounds(HUDRect(MotionDispatcher::instance().cursor->getBounds().x,
                                                               MotionDispatcher::instance().cursor->getBounds().y,
                                                               newCursorW,
                                                               newCursorH));
        resizeCursor = false;
    }

    if (!showKitSelector && kitSelector->isEnabled()) {
        showKitSelector = true;
        sizeChanged = true;
    }
    else if (showKitSelector && !kitSelector->isEnabled()) {
        showKitSelector = false;
        sizeChanged = true;
    }
    
    if (!showPatternSelector && patternSelector->isEnabled()) {
        showPatternSelector = true;
        sizeChanged = true;
    }
    else if (showPatternSelector && !patternSelector->isEnabled()) {
        showPatternSelector = false;
        sizeChanged = true;
    }
    
    if (sizeChanged)
    {
        const int toolbarHeight = 180;
        const int statusBarHeight = 20;
        const int drumSelectorHeight = 100;
        const int tempoControlWidth = 260;
        const int tempoControlHeight = 36;
        const float tutorialWidth = 800.f;
        const float tutorialHeight = 500.f;

        if (tutorial)
            tutorial->setBounds(HUDRect((GLfloat) (Environment::instance().screenW / 2 - tutorialWidth / 2),
                                     (GLfloat) (Environment::instance().screenH / 2 - tutorialHeight / 2),
                                     tutorialWidth,
                                     tutorialHeight));
        
        if (toolbar)
            toolbar->setBounds(HUDRect(0.0f,
                                       (GLfloat) Environment::instance().screenH-toolbarHeight,
                                       (GLfloat) Environment::instance().screenW,
                                       (GLfloat) toolbarHeight));
        
        if (statusBar)
            statusBar->setBounds(HUDRect(0.0f,
                                         0.0f,
                                         (GLfloat) Environment::instance().screenW,
                                         (GLfloat) statusBarHeight));
        
        if (drumSelector)
            drumSelector->setBounds(HUDRect((GLfloat) 5.0f,
                                                (GLfloat) toolbar->getBounds().y + 10,
                                                (GLfloat) Environment::instance().screenW,
                                                (GLfloat) drumSelectorHeight - 5));
        
        if (playAreas.size() != 0)
        {            
            int layout = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("layout", StrikeDetector::kLayout3x2);

            switch (layout)
            {
                case StrikeDetector::kLayout2x1:
                {
                    const float playAreaHeight = (float)(Environment::instance().screenH - toolbarHeight - statusBarHeight);
                    const float playAreaWidth = Environment::instance().screenW / 2.f;
                    playAreas.at(0)->setBounds(HUDRect(0,
                                                    (GLfloat) statusBar->getBounds().top(),
                                                    (GLfloat) playAreaWidth,
                                                    (GLfloat) playAreaHeight));
                
                    playAreas.at(1)->setBounds(HUDRect((GLfloat) playAreaWidth,
                                                     (GLfloat) statusBar->getBounds().top(),
                                                     (GLfloat) playAreaWidth,
                                                     (GLfloat) playAreaHeight));
                    playAreas.at(2)->setBounds(HUDRect(0,0,0,0));
                    playAreas.at(3)->setBounds(HUDRect(0,0,0,0));
                    playAreas.at(4)->setBounds(HUDRect(0,0,0,0));
                    playAreas.at(5)->setBounds(HUDRect(0,0,0,0));
                }
                    break;
                case StrikeDetector::kLayout3x1:
                {
                    const float playAreaHeight = (float)Environment::instance().screenH - toolbarHeight - statusBarHeight;
                    const float playAreaWidth = Environment::instance().screenW / 3.f;
                    playAreas.at(0)->setBounds(HUDRect(0,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(1)->setBounds(HUDRect((GLfloat) playAreaWidth,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(2)->setBounds(HUDRect((GLfloat) playAreaWidth*2,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    playAreas.at(3)->setBounds(HUDRect(0,0,0,0));
                    playAreas.at(4)->setBounds(HUDRect(0,0,0,0));
                    playAreas.at(5)->setBounds(HUDRect(0,0,0,0));
                }
                    break;
                case StrikeDetector::kLayout2x2:
                {
                    const float playAreaHeight = (Environment::instance().screenH - toolbarHeight - statusBarHeight) / 2.f;
                    const float playAreaWidth = Environment::instance().screenW / 2.f;
                    playAreas.at(0)->setBounds(HUDRect(0,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(1)->setBounds(HUDRect((GLfloat) playAreaWidth,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    playAreas.at(2)->setBounds(HUDRect(0,
                                                       (GLfloat) statusBar->getBounds().top() + playAreaHeight,
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(3)->setBounds(HUDRect((GLfloat) playAreaWidth,
                                                       (GLfloat) statusBar->getBounds().top() + playAreaHeight,
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    playAreas.at(4)->setBounds(HUDRect(0,0,0,0));
                    playAreas.at(5)->setBounds(HUDRect(0,0,0,0));
                }
                    break;
                case StrikeDetector::kLayout3x2:
                {
                    const float playAreaHeight = (Environment::instance().screenH - toolbarHeight - statusBarHeight) / 2.f;
                    const float playAreaWidth = Environment::instance().screenW / 3.f;
                    playAreas.at(0)->setBounds(HUDRect(0,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(1)->setBounds(HUDRect((GLfloat) playAreaWidth,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(2)->setBounds(HUDRect((GLfloat) playAreaWidth*2,
                                                       (GLfloat) statusBar->getBounds().top(),
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(3)->setBounds(HUDRect(0,
                                                       (GLfloat) statusBar->getBounds().top() + playAreaHeight,
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(4)->setBounds(HUDRect((GLfloat) playAreaWidth,
                                                       (GLfloat) statusBar->getBounds().top() + playAreaHeight,
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                    
                    playAreas.at(5)->setBounds(HUDRect((GLfloat) playAreaWidth*2,
                                                       (GLfloat) statusBar->getBounds().top() + playAreaHeight,
                                                       (GLfloat) playAreaWidth,
                                                       (GLfloat) playAreaHeight));
                }
                    break;
            }
        }        

        if (trigViewBank)
            trigViewBank->setBounds(HUDRect(0.0f,
                                            (GLfloat) Environment::instance().screenH-70,
                                            (GLfloat) Environment::instance().screenW / 4,
                                            (GLfloat) toolbarHeight));
        
        int side = jmin((int)drumSelector->getBounds().y - statusBarHeight, Environment::instance().screenW);
        int hiddenX = (int) (-side * .85);
        int shownX = (int) (-side / 2.f);
        if (kitSelector)
            kitSelector->setBounds(HUDRect(kitSelector->isEnabled() ? (GLfloat) shownX : (GLfloat) hiddenX,
                                           (GLfloat) statusBarHeight,
                                           (GLfloat) side,
                                           (GLfloat) side));
        
        hiddenX = (int) ((GLfloat) Environment::instance().screenW - side * .15);
        shownX = (int) ((GLfloat) Environment::instance().screenW - side / 2.f);
        if (patternSelector)
            patternSelector->setBounds(HUDRect(patternSelector->isEnabled() ? (GLfloat) shownX : (GLfloat) hiddenX,
                                           (GLfloat) statusBarHeight,
                                           (GLfloat) side,
                                           (GLfloat) side));
        
        if (tempoControl)
            tempoControl->setBounds(HUDRect((GLfloat) Environment::instance().screenW / 2.f - tempoControlWidth / 2.f,
                                            (GLfloat) Environment::instance().screenH - 70 / 2.f - tempoControlHeight / 2.f + 5,
                                            (GLfloat) tempoControlWidth,
                                            (GLfloat) tempoControlHeight));
        
        if (buttonBar)
            buttonBar->setBounds(HUDRect((GLfloat) 0,
                                         (GLfloat) (statusBarHeight + 10),
                                         (GLfloat) Environment::instance().screenW,
                                         (GLfloat)100));
        

#if 0
        layoutPadsLinear();
#endif
        sizeChanged = false;
    }
    
    if (needsPatternListUpdate) {
        populatePatternSelector();
        needsPatternListUpdate = false;
        sizeChanged = true;
    }
    
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
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

    splashBgView->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    splashBgView->draw();
    splashTitleView->setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    splashTitleView->draw();
    
    tutorial->draw();

    MotionDispatcher::instance().cursor->draw();
    
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

//    glDisable(GL_CULL_FACE);
//	tempoSlider.repaint();
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
    
    for (PlayArea* pad : playAreas)
        if (pad->getBounds().contains((GLfloat) e.getPosition().x, (GLfloat) Environment::instance().screenH - e.getPosition().y))
            Drums::instance().NoteOn(pad->getSelectedMidiNote(), 1.f);
    
    tutorial->mouseDown((float) e.getPosition().x, (float) e.getPosition().y);
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
    if (kp.getTextCharacter() == 'h')
    {
        tutorial->setVisible(true);
        splashBgView->setVisible(true);
        for (HUDView* v : views)
            v->setVisible(false);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'm') {
        Drums::instance().getTransportState().toggleMetronome();
        ret = true;
    }
    else if (kp.getKeyCode() == KeyPress::spaceKey) {
        Drums::instance().getTransportState().togglePlayback();
        ret = true;
    }
    else if (kp.getTextCharacter() == 'c') {
        Drums::instance().clear();
        ret = true;
    }
    else if (kp.getTextCharacter() == '1') {
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("layout", StrikeDetector::kLayout2x1);
        sizeChanged = true;
        ret = true;
    }
    else if (kp.getTextCharacter() == '2') {
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("layout", StrikeDetector::kLayout3x1);
        sizeChanged = true;
        ret = true;
    }
    else if (kp.getTextCharacter() == '3') {
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("layout", StrikeDetector::kLayout2x2);
        sizeChanged = true;
        ret = true;
    }
    else if (kp.getTextCharacter() == '4') {
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("layout", StrikeDetector::kLayout3x2);
        sizeChanged = true;
        ret = true;
    }
    else if (kp.getTextCharacter() == 'q') {
        incPadAssociation(0, -1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'w') {
        incPadAssociation(0, 1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'e') {
        incPadAssociation(1, -1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'r') {
        incPadAssociation(1, 1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 't') {
        incPadAssociation(2, -1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'y') {
        incPadAssociation(2, 1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'u') {
        incPadAssociation(3, -1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'i') {
        incPadAssociation(3, 1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'o') {
        incPadAssociation(4, -1);
        ret = true;
    }
    else if (kp.getTextCharacter() == 'p') {
        incPadAssociation(4, 1);
        ret = true;
    }
    else if (kp.getTextCharacter() == '[') {
        incPadAssociation(5, -1);
        ret = true;
    }
    else if (kp.getTextCharacter() == ']') {
        incPadAssociation(5, 1);
        ret = true;
    }
    else if (kp.getKeyCode() == KeyPress::leftKey)
    {
        tempoControl->increment(-1);
        ret = true;
    }
    else if (kp.getKeyCode() == KeyPress::rightKey)
    {
        tempoControl->increment(1);
        ret = true;
    }
    
    AirHarpApplication::getInstance()->getProperties().saveIfNeeded();
    return ret;
}

void MainContentComponent::incPadAssociation(int padNumber, int inc)
{
    jassert(inc == -1 || inc == 1)
    int note = drumSelector->getNoteForPad(padNumber) + inc;
    if (note < 0) note = 15;
    if (note > (int) 15) note = 0;
    
    while (drumSelector->getPadForNote(note) != -1) {
        note = (note + inc) % 16;
        if (note < 0) note = 15;
    }

    drumSelector->setPadAssociation(note, padNumber);
    playAreas.at(padNumber)->setSelectedMidiNote(drumSelector->getNoteForPad(padNumber));
    AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("selectedNote" + String(padNumber), drumSelector->getNoteForPad(padNumber));
}

void MainContentComponent::wheelSelectorChanged(WheelSelector* selector)
{
    if (selector == kitSelector) {
        int selection = kitSelector->getSelection();
        std::shared_ptr<DrumKit> selectedKit = KitManager::GetInstance().GetItem(selection);
        String name = selectedKit->GetName();
        Logger::outputDebugString("kitSelectorChanged - index: " + String(selection) + " name: " + name);
        Uuid uuid = selectedKit->GetUuid();
        String uuidString = uuid.toString();
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("kitName", name);
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("kitUuid", uuidString);
        Drums::instance().setDrumKit(selectedKit);
        for (PlayArea* pad : playAreas)
            pad->setSelectedMidiNote(pad->getSelectedMidiNote());
    }
    else if (selector == patternSelector) {
        int selection = patternSelector->getSelection();
        std::shared_ptr<DrumPattern> selectedPattern = PatternManager::GetInstance().GetItem(selection);
        String name = selectedPattern->GetName();
        Logger::outputDebugString("patternSelectorChanged - index: " + String(selection) + " name: " + name);
        Uuid uuid = selectedPattern->GetUuid();
        String uuidString = uuid.toString();
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("patternName", name);
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("patternUuid", uuidString);
        Drums::instance().setPattern(selectedPattern);
        if (Drums::instance().getTempoSource() == Drums::kPatternTempo)
        {
            tempoControl->setTempo(Drums::instance().getTempo());
        }
        std::shared_ptr<DrumKit> kit = selectedPattern->GetDrumKit();
        if (kit) {
            Drums::instance().setDrumKit(kit);
            int drumKitIndex = KitManager::GetInstance().GetIndexOfItem(selectedPattern->GetDrumKit());
            kitSelector->setSelection(drumKitIndex);
            AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("kitName", kit->GetName());
            AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("kitUuid", kit->GetUuid().toString());
            for (PlayArea* pad : playAreas)
                pad->setSelectedMidiNote(pad->getSelectedMidiNote());
        }
        else
        {
            Logger::outputDebugString("Pattern selected with unknown kit");
        }
    }
}

void MainContentComponent::handleNoteOn(MidiKeyboardState* /*source*/, int /*midiChannel*/, int midiNoteNumber, float /*velocity*/)
{
    if ((unsigned int) midiNoteNumber < pads.size())
    {
        pads.at(midiNoteNumber)->triggerDisplay();
    }

    for (PlayArea* pad : playAreas)
        pad->tap(midiNoteNumber);
}

void MainContentComponent::onFrame(const Leap::Controller& controller)
{
    if (!Environment::instance().ready)
        return;
    
	if (!setPriority)
	{
		setPriority = true;
		Thread::setCurrentThreadPriority(10);
	}

    const Leap::Frame frame = controller.frame();

#if 0 // disabled gestures in favor of simple cursor controlled buttons
    const Leap::GestureList& gestures = frame.gestures();
    handleGestures(gestures);
#endif

    const Leap::HandList& hands = frame.hands();
    const size_t numHands = hands.count();
    
    for (unsigned int h = 0; h < numHands; ++h) {
        const Leap::Hand& hand = hands[h];
        
        std::pair<StrikeDetectorMap::iterator, bool> insertResult = strikeDetectors.insert(std::make_pair(hand.id(), StrikeDetector()));
//        if (insertResult.second)
//            Logger::outputDebugString("Inserted detector for hand id " + String(hand.id()));
        StrikeDetectorMap::iterator iter = insertResult.first;
        StrikeDetector& detector = (*iter).second;
        detector.handMotion(hand);
    }
    
    StrikeDetectorMap::iterator iter = strikeDetectors.begin();
    bool useCursor = true;
    while (iter != strikeDetectors.end())
    {
        if (Time::getCurrentTime() < (*iter).second.getLastStrikeTime() + RelativeTime::milliseconds(500))
            useCursor = false;
        ++iter;
    }
    if (useCursor)
    {
        MotionDispatcher::instance().cursor->setEnabled(true);
    }
    else
    {
        MotionDispatcher::instance().cursor->setEnabled(false);
    }
    
    
}

void MainContentComponent::handleGestures(const Leap::GestureList& gestures)
{
    Leap::GestureList::const_iterator i = gestures.begin();
    Leap::GestureList::const_iterator end = gestures.end();
    if (!gestures.empty())
        isIdle = false;
    for ( ; i != end; ++i)
    {
        const Leap::Gesture& g = *i;
        switch (g.type())
        {
            case Leap::Gesture::TYPE_SWIPE:
            {
                Leap::SwipeGesture swipe(g);
                if (swipe.direction().x > 0 &&  swipe.state() == Leap::Gesture::STATE_START) {
                    if (showPatternSelector) {
                        patternSelector->setEnabled(false);
                        showPatternSelector = false;
                    }
                    else {
                        //kitSelector->setEnabled(true);
                        //showKitSelector = true;
                    }
                    sizeChanged = true;
                }
                else if (swipe.direction().x < 0 &&  swipe.state() == Leap::Gesture::STATE_START) {
                    if (showKitSelector) {
                        kitSelector->setEnabled(false);
                        showKitSelector = false;
                    }
                    else {
                        //patternSelector->setEnabled(true);
                        //showPatternSelector = true;
                    }
                    sizeChanged = true;
                }
                
            }
                break;
            
            case Leap::Gesture::TYPE_KEY_TAP:
            {
                const Leap::KeyTapGesture keyTap(g);
                //handleTapGesture(keyTap.pointable());
            }
                break;
                
            case Leap::Gesture::TYPE_SCREEN_TAP:
            {
                const Leap::ScreenTapGesture screenTap(g);
                //handleTapGesture(screenTap.pointable());
            }
                break;
                
            case Leap::Gesture::TYPE_CIRCLE:
            {
                const Leap::CircleGesture circle(g);
                
                bool isClockwise = circle.normal().z < 0;
                
                if (circle.state() == Leap::Gesture::STATE_START)
                    lastCircleStartTime = Time::currentTimeMillis();
                
                int64 timeDiff = Time::currentTimeMillis() - lastCircleStartTime;
                
                //if (!isClockwise)
                //    printf("Circle CCW - id %d - progress %f\n", circle.id(), circle.progress());
                //else
                //    printf("Circle CW - id %d - progress %f\n", circle.id(), circle.progress());
                                
                if (circle.progress() >= 1.f && circle.progress() < 2.f && isClockwise && lastCircleId != circle.id() && circle.state() == Leap::Gesture::STATE_STOP && timeDiff < 500) {
                    Drums::instance().getTransportState().play();
                    lastCircleId = circle.id();
                }
                else if (circle.progress() >= 2.f && circle.progress() < 3.f&& isClockwise && lastCircleId != circle.id() && circle.state() == Leap::Gesture::STATE_STOP) {
                    Drums::instance().getTransportState().toggleRecording();
                    lastCircleId = circle.id();
                }
                else if (circle.progress() >= 3.f && isClockwise && lastCircleId != circle.id() && circle.state() == Leap::Gesture::STATE_STOP) {
                    Drums::instance().getTransportState().toggleMetronome();
                    lastCircleId = circle.id();
                }
                else if (circle.progress() >= 1.f && circle.progress() < 2.f && !isClockwise && lastCircleId != circle.id() && circle.state() == Leap::Gesture::STATE_STOP) {
                    Drums::instance().getTransportState().pause();
                    lastCircleId = circle.id();
                }
                else if (circle.progress() >= 2.f && circle.progress() < 3.f && !isClockwise && lastCircleId != circle.id() && circle.state() == Leap::Gesture::STATE_STOP) {
                    Drums::instance().resetToZero();
                }
                else if (circle.progress() >= 3.f && !isClockwise && lastCircleId != circle.id() && circle.state() == Leap::Gesture::STATE_STOP) {
                    Drums::instance().clear();
                }
            }
                break;

            default:
                break;
        }
    }
}

void MainContentComponent::handleTapGesture(const Leap::Pointable& /*p*/)
{
#if 0
    if (p.tipPosition().x < 0)
    {
        if (!isTimerRunning(kTimerLeftHandTap)) {
            Drums::instance().NoteOn(playAreaLeft->getSelectedMidiNote(), 1.f);
            startTimer(kTimerLeftHandTap, TAP_TIMEOUT);
        }
        //playAreaLeft->tap(playAreaLeft->getSelectedMidiNote());
    }
    else
    {
        if (!isTimerRunning(kTimerRightHandTap)) {
            Drums::instance().NoteOn(playAreaRight->getSelectedMidiNote(), 1.f);
            startTimer(kTimerRightHandTap, TAP_TIMEOUT);
        }
        //playAreaRight->tap(playAreaRight->getSelectedMidiNote());
    }
#endif
}

void MainContentComponent::timerCallback(int timerId)
{
    switch (timerId) {
        case kTimerShowTutorial:
            tutorial->setVisible(true, 2000);
            stopTimer(kTimerShowTutorial);
            break;
        case kTimerLeftHandTap:
            stopTimer(kTimerLeftHandTap);
            break;
        case kTimerRightHandTap:
            stopTimer(kTimerRightHandTap);
            break;
            
        default:
            break;
    }

}

bool MainContentComponent::checkIdle()
{
    bool retVal = isIdle;
    isIdle = true;
    return retVal;
}

void MainContentComponent::handleMessage(const juce::Message &m)
{
    Message* inMsg = const_cast<Message*>(&m);
    AirHarpApplication::PatternAddedMessage* patternAddedMessage = dynamic_cast<AirHarpApplication::PatternAddedMessage*>(inMsg);
    if (patternAddedMessage)
    {
        needsPatternListUpdate = true;
    }
    
    InitGLMessage* initGLMessage = dynamic_cast<InitGLMessage*>(inMsg);
    if (initGLMessage)
    {
        Logger::outputDebugString("Got InitGLMessage");
        openGLContext.setRenderer (this);
        openGLContext.setComponentPaintingEnabled (true);
        openGLContext.attachTo (*this);
    }
}

void MainContentComponent::actionListenerCallback(const String& message)
{
    if (message.contains("assign/"))
    {
        String padNumberStr = message.trimCharactersAtStart("assign/");
        int padNumber = padNumberStr.getIntValue();
        jassert(padNumber >= 0 && padNumber < (int)playAreas.size());
        PlayArea* playArea = playAreas.at(padNumber);
        playArea->setSelectedMidiNote(drumSelector->getSelection());
        AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("selectedNote" + String(playArea->getId()), drumSelector->getSelection());
        drumSelector->setPadAssociation(drumSelector->getSelection(), playArea->getId());
        for (PlayArea* pad : playAreas)
            pad->enableAssignButton(false);
        drumSelector->setSelection(-1);
        lastDrumSelection = -1;
        MotionDispatcher::instance().setCursorTexture(SkinManager::instance().getSelectedSkin().getTexture("cursor"));
        newCursorW = 20;
        newCursorH = 20;
        resizeCursor = true;
    }
    else if (message.contains("startAssignMode/"))
    {
        String drumNumberStr = message.trimCharactersAtStart("startAssignMode/");
        int drumNumber = drumNumberStr.getIntValue();
        jassert(drumNumber >= 0 && drumNumber < 16);

        // disable clear mode if we're in it
        for (PlayArea* pad : playAreas)
            pad->enableClearButton(false);
        
        bool enableAsignMode = true;
        if (lastDrumSelection == drumNumber)
        {
            drumSelector->setSelection(-1);
            enableAsignMode = false;
            lastDrumSelection = -1;
            MotionDispatcher::instance().setCursorTexture(SkinManager::instance().getSelectedSkin().getTexture("cursor"));
            newCursorW = 20;
            newCursorH = 20;
            resizeCursor = true;
        }
        else
        {
            lastDrumSelection = drumNumber;
            
            // set the cursor to drum texture
            String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
            if (kitUuidString != "Default") {
                Uuid kitUuid(kitUuidString);
                TextureDescription texture = KitManager::GetInstance().GetItem(kitUuid)->GetSample(drumNumber)->GetTexture(false);
                MotionDispatcher::instance().setCursorTexture(texture);
                newCursorW = 50;
                newCursorH = 50;
                resizeCursor = true;
            }
        }
        
        for (PlayArea* pad : playAreas)
            pad->enableAssignButton(enableAsignMode);
        
        buttonBar->resetClearButton();
    }
    else if (message == "cancelAssign")
    {
    }
    else if (message.contains("clear/"))
    {
        String padNumberStr = message.trimCharactersAtStart("clear/");
        int padNumber = padNumberStr.getIntValue();
        jassert(padNumber >= 0 && padNumber < (int)playAreas.size());
        PlayArea* playArea = playAreas.at(padNumber);
        Drums::instance().clearTrack(playArea->getSelectedMidiNote());
        for (PlayArea* pad : playAreas)
            pad->enableClearButton(false);
        buttonBar->resetClearButton();
    }
    else if (message == "clearTrack")
    {
        for (PlayArea* pad : playAreas)
            pad->enableClearButton(true);
        
        // disable assign mode if we're in it
        for (PlayArea* pad : playAreas)
            pad->enableAssignButton(false);
        drumSelector->setSelection(-1);
        lastDrumSelection = -1;
        MotionDispatcher::instance().setCursorTexture(SkinManager::instance().getSelectedSkin().getTexture("cursor"));
        newCursorW = 20;
        newCursorH = 20;
        resizeCursor = true;
    }
    else if (message == "cancelClear")
    {
        for (PlayArea* pad : playAreas)
            pad->enableClearButton(false);
    }
    else if (message == "tutorialDone")
    {
        splashBgView->setVisible(false, 1000);
        for (HUDView* v : views)
            v->setVisible(true);
    }
}

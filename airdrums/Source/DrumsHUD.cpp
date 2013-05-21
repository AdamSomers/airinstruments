#include "DrumsHUD.h"
#include "Drums.h"
#include "MotionServer.h"
#include "SkinManager.h"
#include "Main.h"

DrumsToolbar::DrumsToolbar()
{
    addChild(&resetButton);
    resetButton.addListener(this);

    addChild(&playButton);
    playButton.addListener(this);

    addChild(&recordButton);
    recordButton.addListener(this);

    addChild(&metronomeButton);
    metronomeButton.addListener(this);

    Drums::instance().addTransportListener(this);
    Drums::instance().getTransportState().sendChangeMessage();
}

DrumsToolbar::~DrumsToolbar()
{
}

// HUDView overrides
void DrumsToolbar::setup()
{
    HUDView::setup();
    layoutControls();
}

void DrumsToolbar::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    layoutControls();
}

void DrumsToolbar::layoutControls()
{
    float buttonSpacing = 25;

    float w = 25;
    float h = 40;
    const GLfloat tempoControlHeight = 45;
    const GLfloat tempoControlWidth = 115;
    const GLfloat recordButtonWith = 40;
    const GLfloat metronomeButtonWidth = 36;
    float totalWidth = w + w + recordButtonWith + metronomeButtonWidth + tempoControlWidth;
    float x = bounds.w / 2.f - (totalWidth + (buttonSpacing * 4)) / 2.f;
    float y = 110 + 70 / 2.f - h / 2.f;
    HUDRect r(x, y, w, h);
    resetButton.setBounds(r);
    r.x += w + buttonSpacing;
    playButton.setBounds(r);
    r.x += w + buttonSpacing;
    recordButton.setBounds(HUDRect(r.x, r.y, recordButtonWith, h));
    r.x += w + buttonSpacing + 15;
    metronomeButton.setBounds(HUDRect(r.x, r.y, metronomeButtonWidth, h));
}

void DrumsToolbar::draw()
{
    resetButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("reset_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("reset_off"));
    playButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("play_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("play_off"));
    recordButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("record_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("record_off"));
    metronomeButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("metronome_on"),
                             SkinManager::instance().getSelectedSkin().getTexture("metronome_off"));
    
    resetButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    playButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    recordButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    metronomeButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    
    setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("top_bezel"));

    HUDView::draw();
}

void DrumsToolbar::changeListenerCallback(ChangeBroadcaster* source)
{
    Drums::TransportState* transportstate = dynamic_cast<Drums::TransportState*>(source);
    if (transportstate)
    {
        playButton.setState(transportstate->playing);
        recordButton.setState(transportstate->recording);
        metronomeButton.setState(transportstate->metronomeOn);
    }
}

void DrumsToolbar::buttonStateChanged(HUDButton* b)
{
    if (b == &playButton)
    {
        if (b->getState())
            Drums::instance().getTransportState().play();
        else
            Drums::instance().getTransportState().pause();
    }
    else if (b == &recordButton)
    {
        Drums::instance().getTransportState().record(b->getState());
    }
    else if (b == &metronomeButton)
    {
        Drums::instance().getTransportState().metronome(b->getState());
        AirHarpApplication* app = AirHarpApplication::getInstance();
        ApplicationProperties& props = app->getProperties();
        props.getUserSettings()->setValue("metronome", b->getState());

    }
    else if (b == &resetButton)
    {
        Drums::instance().resetToZero();
        startTimer(200);
    }
}

void DrumsToolbar::timerCallback()
{
    stopTimer();
    resetButton.setState(false);
}

StatusBar::StatusBar()
: indicator(HUDButton(0))
{
    //indicator.setEditable(false)
    addChild(&indicator);
    GLfloat color [] = { 0.95f, 0.95f, 0.95f, 1.f };
    setDefaultColor(color);
}

StatusBar::~StatusBar()
{
    MotionDispatcher::instance().removeListener(*this);
}

// HUDView overrides
void StatusBar::setup()
{
    HUDView::setup();
    layoutControls();

    MotionDispatcher::instance().addListener(*this);
}

void StatusBar::layoutControls()
{
    float w = 10;
    float h = 10;
    float x = bounds.w - 15;
    float y = bounds.h / 2.f - h / 2.f;
    HUDRect r(x, y, w, h);
    indicator.setBounds(r);
}

void StatusBar::draw()
{
    indicator.setTextures(SkinManager::instance().getSelectedSkin().getTexture("leapStatus_on"),
                           SkinManager::instance().getSelectedSkin().getTexture("leapStatus_off"));
    HUDView::draw();
}

void StatusBar::onInit(const Leap::Controller& /*controller*/)
{
}

void StatusBar::onConnect(const Leap::Controller& /*controller*/)
{
    indicator.setState(true);
}

void StatusBar::onDisconnect(const Leap::Controller& /*controller*/)
{
    indicator.setState(false);
}

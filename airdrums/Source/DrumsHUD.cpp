#include "DrumsHUD.h"
#include "Drums.h"
#include "MotionServer.h"
#include "SkinManager.h"

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

    GLfloat color[] = { .67f, .67f, .67f, 1.f };
    setDefaultColor(color);
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

void DrumsToolbar::layoutControls()
{
    float buttonSpacing = 10;

    float w = 50;
    float h = 50;
    float x = bounds.w - (w + buttonSpacing) * 4;
    float y = bounds.h / 2.f - h / 2.f;
    HUDRect r(x, y, w, h);
    resetButton.setBounds(r);
    r.x += w + buttonSpacing;
    playButton.setBounds(r);
    r.x += w + buttonSpacing;
    recordButton.setBounds(r);
    r.x += w + buttonSpacing;
    metronomeButton.setBounds(r);
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
    GLfloat color [] = { 0.67f, 0.67f, 0.67f, 1.f };
    setDefaultColor(color);
}

StatusBar::~StatusBar()
{
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
    float w = 20;
    float h = 20;
    float x = bounds.w - 35;
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

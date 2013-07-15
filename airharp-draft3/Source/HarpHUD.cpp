#include "HarpHUD.h"
#include "Harp.h"
#include "MotionServer.h"
#include "GfxTools.h"
#include "SkinManager.h"
#include "Main.h"

HarpToolbar::HarpToolbar()
{
    setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 7; ++i)
    {
        TextHUDButton* b = new TextHUDButton();
        b->setId(i);
        b->setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
        b->setBackgroundColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, .8f),
                              Colour::fromFloatRGBA(.3f, .3f, .3f, .5f));
        b->setTextColor(Colour::fromFloatRGBA(.2f, .2f, .2f, 1.f),
                        Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
        b->addListener(this);
        buttons.push_back(b);
        addChild(b);
    }
    
    settingsButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    settingsButton.setBackgroundColor(Colour::fromFloatRGBA(.3f, .3f, .3f, .5f),
                          Colour::fromFloatRGBA(.3f, .3f, .3f, .5f));
    settingsButton.setTextColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f),
                    Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
    settingsButton.addListener(this);
    settingsButton.setText(StringArray("Settings"), StringArray("Settings"));
    addChild(&settingsButton);
    
    helpButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    helpButton.setBackgroundColor(Colour::fromFloatRGBA(.3f, .3f, .3f, .5f),
                                      Colour::fromFloatRGBA(.3f, .3f, .3f, .5f));
    helpButton.setTextColor(Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f),
                                Colour::fromFloatRGBA(1.f, 1.f, 1.f, 1.f));
    helpButton.addListener(this);
    helpButton.setText(StringArray("Help"), StringArray("Help"));
    addChild(&helpButton);
}

HarpToolbar::~HarpToolbar()
{
    for (TextHUDButton* b : buttons)
        delete b;
}

// HUDView overrides
void HarpToolbar::setup()
{
    HUDView::setup();   
    layoutControls();
}

void HarpToolbar::layoutControls()
{
    int numButtons = buttons.size();
    float buttonWidth = 60;
    float buttonHeight = 60;
    float xmin = 50;
    float xmax = 400;
    float step = buttonWidth + 5;
    float y = (bounds.h / 2.f + 15)- buttonHeight / 2.f;
    HUDRect r(xmin, y, buttonWidth, buttonHeight);
    for (TextHUDButton* b : buttons)
    {
        b->setBounds(r);
        r.x += step;
    }
    
    HUDRect buttonRect(getBounds().w - buttonWidth - 50,
                       y,
                       buttonWidth,
                       buttonHeight);
    settingsButton.setBounds(buttonRect);
    buttonRect.x -= buttonWidth + 10;
    helpButton.setBounds(buttonRect);
}

void HarpToolbar::draw()
{
    GLfloat color [] = { 1.f, 1.f, 1.f, 1.f };

    setDefaultColor(color);
    setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("bezel_top0"));

    HUDView::draw();
}

void HarpToolbar::loadTextures()
{
    HUDView::loadTextures();
}

void HarpToolbar::setButtonTextures(TextureDescription on, TextureDescription off)
{
    for (HUDButton* b : buttons)
        b->setTextures(on, off);
}

void HarpToolbar::buttonStateChanged(HUDButton* b)
{
    bool state = b->getState();
    
    if (b == &settingsButton)
        sendActionMessage("settingsMode");
    else if (b == &helpButton)
        sendActionMessage("showHelp");
    else
    {    
        Harp* h = HarpManager::instance().getHarp(0);
        
        if (h->getChordMode())
        {
            if (state) {
                h->selectChord(b->getId());
                AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("chordSelected"+String(b->getId()), true);
            }
            else if (h->getNumSelectedChords() > 1) {
                h->deSelectChord(b->getId());
                AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("chordSelected"+String(b->getId()), false);
            }
        }
        else
        {
            if (state)
            {
                for (TextHUDButton* button : buttons)
                {
                    if (button != b)
                        button->setState(false, false);
                }
            }
            else
                b->setState(true, false);
            
            h->SetScale(b->getId());
            AirHarpApplication::getInstance()->getProperties().getUserSettings()->setValue("selectedScale", b->getId());
        }
        sendChangeMessage();
        sendActionMessage("scaleChanged");
    }
}

void HarpToolbar::updateButtons()
{
    Harp* h = HarpManager::instance().getHarp(0);
    
    if (h->getChordMode())
    {
        for (TextHUDButton* b : buttons)
        {
            if (h->isChordSelected(b->getId()))
                b->setState(true);
            else
                b->setState(false);
        }
    }
    else
    {
        for (TextHUDButton* b : buttons)
        {
            if (h->getSelectedScale() == b->getId())
                b->setState(true);
            else
                b->setState(false);
        }
    }
    
    updateButtonText();
}

void HarpToolbar::updateButtonText()
{
    Harp* h = HarpManager::instance().getHarp(0);
    if (h->getChordMode())
    {
        buttons.at(0)->setText(StringArray("I"),StringArray("I"));
        buttons.at(1)->setText(StringArray("II"),StringArray("II"));
        buttons.at(2)->setText(StringArray("III"),StringArray("III"));
        buttons.at(3)->setText(StringArray("IV"),StringArray("IV"));
        buttons.at(4)->setText(StringArray("V"),StringArray("V"));
        buttons.at(5)->setText(StringArray("VI"),StringArray("VI"));
        buttons.at(6)->setText(StringArray("VII"),StringArray("VII"));
    }
    else
    {
        StringArray arr;
        buttons.at(0)->setText(StringArray("Major"),StringArray("Major"));
        buttons.at(1)->setText(StringArray("Minor"),StringArray("Minor"));
        arr.add("Pent.");
        arr.add("Major");
        buttons.at(2)->setText(arr,arr);
        arr.clear();
        arr.add("Pent.");
        arr.add("Minor");
        buttons.at(3)->setText(arr,arr);
        arr.clear();
        arr.add("Whole");
        arr.add("Tone");
        buttons.at(4)->setText(arr,arr);
        buttons.at(5)->setText(StringArray("Chinese"),StringArray("Chinese"));
        buttons.at(6)->setText(StringArray("Custom"),StringArray("Custom"));
    }
}

StatusBar::StatusBar()
: indicator(HUDButton(0))
{
    setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //indicator.setEditable(false)
    addChild(&indicator);
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
    float y = 10 - h / 2.f;
    HUDRect r(x, y, w, h);
    indicator.setBounds(r);
}

void StatusBar::setIndicatorTextures(TextureDescription on, TextureDescription off)
{
    indicator.setTextures(on, off);
}

void StatusBar::draw()
{
    GLfloat color [] = { 1.f, 1.f, 1.f, 1.f };
    setDefaultColor(color);
    setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("bezel_bottom0"));

    HUDView::draw();
}

void StatusBar::onInit(const Leap::Controller& controller)
{
}

void StatusBar::onConnect(const Leap::Controller& controller)
{
    indicator.setState(true);
}

void StatusBar::onDisconnect(const Leap::Controller& controller)
{
    indicator.setState(false);
}

ChordRegion::ChordRegion()
: id(0)
, isActive(false)
{
    setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    setMultiplyAlpha(true);
    addChild(&label);
    GLfloat color[] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
}

ChordRegion::~ChordRegion()
{
    
}

void ChordRegion::setBounds(const HUDRect& r)
{
    HUDView::setBounds(r);
    float labelHeight = jmin(r.h, 40.f);
    
    label.setBounds(HUDRect(20,
                            r.h / 2.f - labelHeight / 2.f,
                            labelHeight,
                            labelHeight));
}

void ChordRegion::setId(int inId)
{
    id = inId;
    String text;
    switch (id)
    {
        case 0:
            text = "I";
            break;
        case 1:
            text = "II";
            break;
        case 2:
            text = "III";
            break;
        case 3:
            text = "IV";
            break;
        case 4:
            text = "V";
            break;
        case 5:
            text = "VI";
            break;
        case 6:
            text = "VII";
            break;
    }
    label.setText(text);
}

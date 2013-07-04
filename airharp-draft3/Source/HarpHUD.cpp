#include "HarpHUD.h"
#include "Harp.h"
#include "MotionServer.h"
#include "GfxTools.h"
#include "SkinManager.h"

HarpToolbar::HarpToolbar()
{
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
    float buttonWidth = 50;
    float buttonHeight = 50;
    float xmin = 50;
    float xmax = 400;
    float totalButtonWidth = numButtons * buttonWidth;
    float emptySpace = (xmax - xmin) - totalButtonWidth;
    float step = (emptySpace / (numButtons-1)) + buttonWidth;
    if (step < buttonWidth + 1)
        step = buttonWidth + 1;
    float y = (bounds.h / 2.f + 10)- buttonHeight / 2.f;
    HUDRect r(xmin, y, buttonWidth, buttonHeight);
    for (TextHUDButton* b : buttons)
    {
        b->setBounds(r);
        r.x += step;
    }
    
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
    Harp* h = HarpManager::instance().getHarp(0);
    
    if (h->getChordMode())
    {
        if (state)
            h->selectChord(b->getId());
        else if (h->getNumSelectedChords() > 1)
            h->deSelectChord(b->getId());
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
    }
    sendChangeMessage();
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
        buttons.at(6)->setText(StringArray("Exotic"),StringArray("Exotic"));
    }
}

StatusBar::StatusBar()
: indicator(HUDButton(0))
{
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
//    indicator.setTextures(on, off);
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
, fade(0.f)
{
    
}

ChordRegion::~ChordRegion()
{
    
}

void ChordRegion::setup()
{    
    M3DVector3f imageVerts[4] = {
        0, bounds.y, 0.f,
        bounds.h, bounds.y, 0.f,
        0, bounds.y + bounds.h, 0.f,
        bounds.h, bounds.y + bounds.h, 0.f
    };
    
    M3DVector2f texCoords[4] = {
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 0.f
    };
    
    if (!didSetup)
        imageBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    imageBatch.CopyVertexData3f(imageVerts);
    imageBatch.CopyTexCoordData2f(texCoords, 0);
    if (!didSetup)
        imageBatch.End();
    
    HUDView::setup();
}

void ChordRegion::draw()
{
    GLfloat texColor[4] = { 1.f, 1.f, 1.f, fade };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_BLEND);

    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), texColor, 0);
    glLineWidth(1.f);
    imageBatch.Draw();

    if (isActive && fade < 1.f)
    {
        fade += 0.3f;
        if (fade > .7f) fade = .7f;
    }
    if (!isActive && fade > 0.f)
    {
        fade -= 0.11f;
        if (fade < 0.f) fade = 0.f;
    }
}

void ChordRegion::loadTextures()
{
    textureID = SkinManager::instance().getSelectedSkin().getTexture(String(id+1)).textureId;
}

void ChordRegion::setActive(bool shouldBeActive)
{
    isActive = shouldBeActive;
}

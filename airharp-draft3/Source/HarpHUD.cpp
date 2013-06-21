#include "HarpHUD.h"
#include "Harp.h"
#include "MotionServer.h"
#include "GfxTools.h"
#include "SkinManager.h"

HarpToolbar::HarpToolbar()
{
    for (int i = 0; i < 7; ++i)
    {
        HUDButton* b = new HUDButton(i);
        b->addListener(this);
        buttons.push_back(b);
        addChild(b);
    }
}

HarpToolbar::~HarpToolbar()
{
    for (HUDButton* b : buttons)
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
    float buttonWidth = 30;
    float buttonHeight = 30;
    float xmin = 50;
    float xmax = 400;
    float totalButtonWidth = numButtons * buttonWidth;
    float emptySpace = (xmax - xmin) - totalButtonWidth;
    float step = (emptySpace / (numButtons-1)) + buttonWidth;
    if (step < buttonWidth + 1)
        step = buttonWidth + 1;
    float y = (bounds.h / 2.f + 10)- buttonHeight / 2.f;
    HUDRect r(xmin, y, buttonWidth, buttonHeight);
    for (HUDButton* b : buttons)
    {
        b->setBounds(r);
        r.x += step;
    }
    
}

void HarpToolbar::draw()
{
    GLfloat color [] = { 0.67f, 0.67f, 0.67f, 1.f };

    setDefaultColor(color);
//    setDefaultTexture(SkinManager::instance().getSkin().bezelTop);

    HUDView::draw();
}

void HarpToolbar::loadTextures()
{
    HUDView::loadTextures();
}

void HarpToolbar::setButtonTextures(GLuint on, GLuint off)
{
//    for (HUDButton* b : buttons)
//        b->setTextures(on, off);
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
            for (HUDButton* button : buttons)
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
        for (HUDButton* b : buttons)
        {
            if (h->isChordSelected(b->getId()))
                b->setState(true);
            else
                b->setState(false);
        }
    }
    else
    {
        for (HUDButton* b : buttons)
        {
            if (h->getSelectedScale() == b->getId())
                b->setState(true);
            else
                b->setState(false);
        }
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
}

// HUDView overrides
void StatusBar::setup()
{
    HUDView::setup();
    layoutControls();
    MotionDispatcher::instance().controller.addListener(*this);
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

void StatusBar::setIndicatorTextures(GLuint on, GLuint off)
{
//    indicator.setTextures(on, off);
}

void StatusBar::draw()
{
    GLfloat color [] = { 0.67f, 0.67f, 0.67f, 1.f };
    setDefaultColor(color);
//    setDefaultTexture(SkinManager::instance().getSkin().bezelBottom);

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
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    switch (id) {
        case 0:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_1_png, BinaryData::_1_pngSize));
            break;
        case 1:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_2_png, BinaryData::_2_pngSize));
            break;
        case 2:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_3_png, BinaryData::_3_pngSize));
            break;
        case 3:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_4_png, BinaryData::_4_pngSize));
            break;
        case 4:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_5_png, BinaryData::_5_pngSize));
            break;
        case 5:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_6_png, BinaryData::_6_pngSize));
            break;
        case 6:
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::_7_png, BinaryData::_7_pngSize));
            break;
        default:
            break;
    }
}

void ChordRegion::setActive(bool shouldBeActive)
{
    isActive = shouldBeActive;
}

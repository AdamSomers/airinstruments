#include "HarpHUD.h"
#include "Harp.h"
#include "MotionServer.h"

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
    
    M3DVector3f verts[4] = {
        bounds.x, bounds.y, 0.f,
        bounds.x + bounds.w, bounds.y, 0.f,
        bounds.x, bounds.y + bounds.h, 0.f,
        bounds.x + bounds.w, bounds.y + bounds.h, 0.f
    };
    
    batch.Begin(GL_TRIANGLE_STRIP, 4);
    batch.CopyVertexData3f(verts);
    batch.End();
    
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
    float y = bounds.h / 2.f - buttonHeight / 2.f;
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
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    glLineWidth(1.f);
    batch.Draw();
    HUDView::draw();
}

void HarpToolbar::buttonStateChanged(HUDButton* b)
{
    bool state = b->getState();
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
    
    HarpManager::instance().getHarp(0)->SetScale(b->getId());
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
    
    M3DVector3f verts[4] = {
        bounds.x, bounds.y, 0.f,
        bounds.x + bounds.w, bounds.y, 0.f,
        bounds.x, bounds.y + bounds.h, 0.f,
        bounds.x + bounds.w, bounds.y + bounds.h, 0.f
    };
    
    batch.Begin(GL_TRIANGLE_STRIP, 4);
    batch.CopyVertexData3f(verts);
    batch.End();
    
    layoutControls();
    
    MotionDispatcher::instance().controller.addListener(*this);
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
    GLfloat color [] = { 0.67f, 0.67f, 0.67f, 1.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    glLineWidth(1.f);
    batch.Draw();
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

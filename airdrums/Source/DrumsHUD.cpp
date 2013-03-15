#include "DrumsHUD.h"
#include "Drums.h"
#include "MotionServer.h"

DrumsToolbar::DrumsToolbar()
{
}

DrumsToolbar::~DrumsToolbar()
{
}

// HUDView overrides
void DrumsToolbar::setup()
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

void DrumsToolbar::layoutControls()
{    
}

void DrumsToolbar::draw()
{
    GLfloat color [] = { 0.67f, 0.67f, 0.67f, 1.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
    glLineWidth(1.f);
    batch.Draw();
    HUDView::draw();
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

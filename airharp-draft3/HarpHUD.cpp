#include "HarpHUD.h"


Toolbar::Toolbar()
{
    for (int i = 0; i < 7; ++i)
    {
        HUDButton* b = new HUDButton;
        buttons.push_back(b);
        addChild(b);
    }
}

Toolbar::~Toolbar()
{
    for (HUDButton* b : buttons)
        delete b;
}

// HUDView overrides
void Toolbar::setup()
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

void Toolbar::layoutControls()
{
    int numButtons = buttons.size();
    float buttonWidth = 25;
    float buttonHeight = 25;
    float xmin = 50;
    float xmax = bounds.w - 50;
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

void Toolbar::draw()
{
    GLfloat vRed [] = { 1.f, 0.f, 0.f, 1.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), vRed);
    glLineWidth(1.f);
    batch.Draw();
    HUDView::draw();
    
}

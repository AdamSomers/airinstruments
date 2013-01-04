#include "HarpHUD.h"


Toolbar::Toolbar()
{
    addChild(&b1);
    addChild(&b2);
    addChild(&b3);
    addChild(&b4);
    b1.setBounds(HUDRect(.02,.02,.08,.08));
    b2.setBounds(HUDRect(.104,.02,.1,.08));
    b3.setBounds(HUDRect(.02,.106,.07,.07));
    b4.setBounds(HUDRect(.102,.106,.05,.05));
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
}

void Toolbar::draw()
{
    GLfloat vRed [] = { 1.f, 0.f, 0.f, 1.f };
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), vRed);
    glLineWidth(1.f);
    batch.Draw();
    HUDView::draw();
    
}

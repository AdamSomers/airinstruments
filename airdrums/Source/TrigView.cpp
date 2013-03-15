#include "TrigView.h"
#include "GfxTools.h"

TrigView::TrigView()
: onTextureID(-1)
, offTextureID(-1)
, fade(0.f)
{
}

TrigView::~TrigView()
{
}

void TrigView::setup()
{
    HUDView::setup();
    
    M3DVector3f verts[4] = {
        bounds.x, bounds.y, 0.f,
        bounds.x + bounds.w, bounds.y, 0.f,
        bounds.x, bounds.y + bounds.h, 0.f,
        bounds.x + bounds.w, bounds.y + bounds.h, 0.f
    };
    
    M3DVector3f normals[4] = {
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f,
        0.f, 0.f, 1.f
    };
    
    M3DVector2f texCoords[4] = {
        0.f, 1.f,
        1.f, 1.f,
        0.f, 0.f,
        1.f, 0.f
    };
    
    batch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    batch.CopyVertexData3f(verts);
    batch.CopyTexCoordData2f(texCoords, 0);
    batch.CopyNormalDataf(normals);
    batch.End();
}

void TrigView::loadTextures()
{
    glGenTextures(1, &onTextureID);
    glBindTexture(GL_TEXTURE_2D, onTextureID);
    GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::led1_on_png, BinaryData::pad1_on_pngSize));
    glGenTextures(1, &offTextureID);
    glBindTexture(GL_TEXTURE_2D, offTextureID);
    GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::led1_off_png, BinaryData::pad1_off_pngSize));
}

void TrigView::draw()
{
    GLfloat onTexColor[4] = { 1.f, 1.f, 1.f, fade };
    GLfloat offTexColor[4] = { 1.f, 1.f, 1.f, 1.f - fade };
    
    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onTexColor, 0);
    glLineWidth(1.f);
    batch.Draw();
    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offTexColor, 0);
    batch.Draw();
    
    if (fade > 0.f)
    {
        fade -= 0.11f;
        if (fade < 0.f) fade = 0.f;
    }
}

void TrigView::trigger()
{
    fade = 1.f;
}

TrigViewBank::TrigViewBank()
: textureID(-1)
{
    for (int i = 0; i < 16; ++i) {
        TrigView* tv = new TrigView;
        trigViews.push_back(tv);
        addChild(tv);
    }
}

TrigViewBank::~TrigViewBank()
{
    
}

void TrigViewBank::setup()
{
    HUDView::setup();
}

void TrigViewBank::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    int trigWidth = 20;
    int step = b.w / trigViews.size();
    int x = 0;
    int y = 0;
    for (int i = 0; i < trigViews.size(); ++i) {
        trigViews.at(i)->setBounds(HUDRect(x, y, trigWidth, trigWidth));
        x += step;
        if (i == 7) {
            y = 10;
            x = 0;
        }
    }
}

void TrigViewBank::draw()
{
    HUDView::draw();
}

void TrigViewBank::loadTextures()
{
    HUDView::loadTextures();
}

void TrigViewBank::trigger(int midiNote)
{
    if (midiNote < trigViews.size())
        trigViews.at(midiNote)->trigger();
}

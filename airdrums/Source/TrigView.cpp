#include "TrigView.h"
#include "GfxTools.h"
#include "SkinManager.h"
#include "Drums.h"
#include "KitManager.h"
#include "Main.h"

TrigView::TrigView(int inId)
: fade(0.f)
, id(inId)
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
    
    ledBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    ledBatch.CopyVertexData3f(verts);
    ledBatch.CopyTexCoordData2f(texCoords, 0);
    ledBatch.CopyNormalDataf(normals);
    ledBatch.End();
    
    LRBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    LRBatch.CopyVertexData3f(verts);
    LRBatch.CopyTexCoordData2f(texCoords, 0);
    LRBatch.CopyNormalDataf(normals);
    LRBatch.End();
    
    categoryBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);
    categoryBatch.CopyVertexData3f(verts);
    categoryBatch.CopyTexCoordData2f(texCoords, 0);
    categoryBatch.CopyNormalDataf(normals);
    categoryBatch.End();
}

void TrigView::loadTextures()
{
}

void TrigView::draw()
{
    GLfloat onTexColor[4] = { 1.f, 1.f, 1.f, fade };
    GLfloat offTexColor[4] = { 1.f, 1.f, 1.f, 1.f - fade };
    
    GLuint onTextureID = SkinManager::instance().getSelectedSkin().getTexture("led_on");
    GLuint offTextureID = SkinManager::instance().getSelectedSkin().getTexture("led_off");
    
    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onTexColor, 0);
    glLineWidth(1.f);
    ledBatch.Draw();
    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offTexColor, 0);
    ledBatch.Draw();
    
    if (fade > 0.f)
    {
        fade -= 0.11f;
        if (fade < 0.f) fade = 0.f;
    }
    
    String category = KitManager::GetInstance().GetItem(0)->GetSample(id)->GetCategory();
    GLuint categoryTextureId = SkinManager::instance().getSelectedSkin().getTexture("Trig_" + category);
    glBindTexture(GL_TEXTURE_2D, categoryTextureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewMatrix(), 0);
    categoryBatch.Draw();
    
    int left = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedNoteLeft");
    int right = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("selectedNoteRight");
    if (left == id)
    {
        GLuint LTextureId = SkinManager::instance().getSelectedSkin().getTexture("Trig_L");
        glBindTexture(GL_TEXTURE_2D, LTextureId);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewMatrix(), 0);
        LRBatch.Draw();
        
    }
    if (right == id)
    {
        GLuint RTextureId = SkinManager::instance().getSelectedSkin().getTexture("Trig_R");
        glBindTexture(GL_TEXTURE_2D, RTextureId);
        Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, Environment::instance().transformPipeline.GetModelViewMatrix(), 0);
        LRBatch.Draw();
    }
}

void TrigView::trigger()
{
    fade = 1.f;
}

TrigViewBank::TrigViewBank()
: textureID((GLuint) -1)
{
    for (int i = 0; i < 16; ++i) {
        TrigView* tv = new TrigView(i);
        trigViews.push_back(tv);
        addChild(tv);
    }
    Drums::instance().playbackState.addListener(this);
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
    int trigWidth = 23;
    int trigHeight = 32;
    int step = trigWidth;
    int x = 15;
    int y = 40;
    for (unsigned int i = 0; i < trigViews.size(); ++i) {
        trigViews.at(i)->setBounds(HUDRect((GLfloat) x, (GLfloat) y, (GLfloat) trigWidth, (GLfloat) trigHeight));
        x += step;
        if (i == 7) {
            y -= trigHeight;
            x = 15;
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
    if (midiNote < (int) trigViews.size())
        trigViews.at(midiNote)->trigger();
}

void TrigViewBank::handleNoteOn(MidiKeyboardState* /*source*/, int /*midiChannel*/, int midiNoteNumber, float /*velocity*/)
{
    trigger(midiNoteNumber);
}
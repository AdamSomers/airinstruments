#include "PlayArea.h"
#include "Drums.h"
#include "GfxTools.h"

PlayArea::PlayArea()
: onTextureID(-1)
, offTextureID(-1)
, fade(0.f)
, selectedMidiNote(0)
{
    offColor[0] = .3f;
    offColor[1] = .3f;
    offColor[2] = .3f;
    offColor[3] = 1.f;
    
    onColor[0] = 207.f / 255.f;
    onColor[1] = 215.f / 255.f;
    onColor[2] = 220.f / 255.f;
    onColor[3] = .5f;
}

PlayArea::~PlayArea()
{
    
}

void PlayArea::setup()
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

void PlayArea::draw()
{
    GLfloat color[4] =  { (onColor[0] * fade) + (offColor[0] * (1.f-fade)),
                          (onColor[1] * fade) + (offColor[1] * (1.f-fade)),
                          (onColor[2] * fade) + (offColor[2] * (1.f-fade)),
                          (onColor[3] * fade) + (offColor[3] * (1.f-fade)) };
    
//    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_FLAT, Environment::instance().transformPipeline.GetModelViewMatrix(), color);
//    batch.Draw();
    
    GLfloat onTexColor[4] = { 1.f, 1.f, 1.f, fade };
    GLfloat offTexColor[4] = { 1.f, 1.f, 1.f, 1.f - fade };
    
    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onTexColor, 0);
    glLineWidth(1.f);
    batch.Draw();
    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offTexColor, 0);
    batch.Draw();
    
//    if (fade < 1.f)
//    {
//        fade += 0.3f;
//        if (fade > 1.f) fade = 1.f;
//    }

    if (fade > 0.f)
    {
        fade -= 0.11f;
        if (fade < 0.f) fade = 0.f;
    }
}

void PlayArea::tap(int midiNote)
{
    if (midiNote == selectedMidiNote)
        fade = 1.f;
}

void PlayArea::setSelectedMidiNote(int note)
{
    if (note >= Drums::instance().getNumNotes())
        note = 0;
    else if (note < 0)
        note = Drums::instance().getNumNotes() - 1;
    selectedMidiNote = note;
}

void PlayArea::loadTextures()
{
        glGenTextures(1, &onTextureID);
        glBindTexture(GL_TEXTURE_2D, onTextureID);
        GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::pad1_on_png, BinaryData::pad1_on_pngSize));
        glGenTextures(1, &offTextureID);
        glBindTexture(GL_TEXTURE_2D, offTextureID);
        GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (BinaryData::pad1_off_png, BinaryData::pad1_off_pngSize));
}
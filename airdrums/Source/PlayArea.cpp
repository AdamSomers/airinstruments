#include "PlayArea.h"
#include "Drums.h"
#include "GfxTools.h"
#include "SkinManager.h"

PlayArea::PlayArea()
: fade(0.f)
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
}

void PlayArea::draw()
{
    GLfloat onTexColor[4] = { 1.f, 1.f, 1.f, fade };
    GLfloat offTexColor[4] = { 1.f, 1.f, 1.f, 1.f - fade };
    
    GLuint onTextureID = SkinManager::instance().getSelectedSkin().getTexture("pad_on");
    GLuint offTextureID = SkinManager::instance().getSelectedSkin().getTexture("pad_off");

    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onTexColor, 0);
    glLineWidth(1.f);
    defaultBatch.Draw();
    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offTexColor, 0);
    defaultBatch.Draw();

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
}
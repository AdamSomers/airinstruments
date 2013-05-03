#include "PlayArea.h"
#include "Drums.h"
#include "GfxTools.h"
#include "SkinManager.h"

PlayArea::PlayArea()
: fade(0.f)
, selectedMidiNote(0)
{
    offColor[0] = 1.f;
    offColor[1] = .5f;
    offColor[2] = .5f;
    offColor[3] = 1.f;
    
    onColor[0] = 1.f;
    onColor[1] = .5f;
    onColor[2] = .5f;
    onColor[3] = 1.f;
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
    onColor[3] = fade;
    offColor[3] = 1.f - fade;
    
    GLuint onTextureID = SkinManager::instance().getSelectedSkin().getTexture("pad_on");
    GLuint offTextureID = SkinManager::instance().getSelectedSkin().getTexture("pad_off");

    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), onColor, 0);
    glLineWidth(1.f);
    defaultBatch.Draw();

    GLint blendSrc;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    GLint blendDst;
    glGetIntegerv(GL_BLEND_DST, &blendDst);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), offColor, 0);
    defaultBatch.Draw();

    glBlendFunc(blendSrc, blendDst);

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

void PlayArea::setColor(const Colour& color)
{
    onColor[0] = color.getFloatRed();
    onColor[1] = color.getFloatGreen();
    onColor[2] = color.getFloatBlue();
    onColor[3] = color.getFloatAlpha();
    offColor[0] = color.getFloatRed();
    offColor[1] = color.getFloatGreen();
    offColor[2] = color.getFloatBlue();
    offColor[3] = color.getFloatAlpha();
}

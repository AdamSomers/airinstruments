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
}

void TrigView::loadTextures()
{
}

void TrigView::draw()
{
    GLfloat onTexColor[4] = { 1.f, 1.f, 1.f, fade };
    GLfloat offTexColor[4] = { 1.f, 1.f, 1.f, 1.f - fade };
    
    String category = "BassDrum";
    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
    if (kitUuidString != "Default") {
        Uuid kitUuid(kitUuidString);
        category = KitManager::GetInstance().GetItem(kitUuid)->GetSample(id)->GetCategory();
    }
    else
        category = KitManager::GetInstance().GetItem(0)->GetSample(id)->GetCategory();

    GLuint onTextureID = SkinManager::instance().getSelectedSkin().getTexture("Trig_" + category + "_on");
    GLuint offTextureID = SkinManager::instance().getSelectedSkin().getTexture("Trig_" + category);
    
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

void TrigView::trigger()
{
    fade = 1.f;
}

TrigViewBank::TrigViewBank()
: textureID((GLuint) -1)
{
    for (int i = 0; i < 16; ++i) {
        SharedPtr<TrigView> tv(new TrigView(i));
        trigViews.push_back(tv);
        addChild(tv.get());
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
    int trigWidth = 15;
    int trigHeight = 15;
    int step = trigWidth + 7;
    int x = 15;
    int y = 40;
    for (unsigned int i = 0; i < trigViews.size(); ++i) {
        trigViews.at(i)->setBounds(HUDRect((GLfloat) x, (GLfloat) y, (GLfloat) trigWidth, (GLfloat) trigHeight));
        x += step;
        if (i == 7) {
            y -= trigHeight + 7;
            x = 15;
        }
    }
}

void TrigViewBank::draw()
{
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
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
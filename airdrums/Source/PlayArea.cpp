#include "PlayArea.h"
#include "Drums.h"
#include "GfxTools.h"
#include "SkinManager.h"
#include "Main.h"
#include "KitManager.h"

#define FADE_TIME 300

PlayArea::PlayArea(int inId)
: selectedMidiNote(0)
, iconRotation(0.f)
, id(inId)
{
    offColor[0] = 1.f;
    offColor[1] = .5f;
    offColor[2] = .5f;
    offColor[3] = 1.f;
    
    onColor[0] = 1.f;
    onColor[1] = .5f;
    onColor[2] = .5f;
    onColor[3] = 1.f;
    
    addChild(&icon);
    
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
    
    addChild(&assignButton);
    assignButton.setText(StringArray("Assign"), StringArray("Assign"));
    assignButton.setVisible(false, 0);
    assignButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    assignButton.addListener(this);
}

PlayArea::~PlayArea()
{
}

void PlayArea::setBounds(const HUDRect& r)
{
    float w = jmin(r.w, r.h) / 2.f;
    float x = r.w / 2.f - w / 2.f;
    float y = r.h / 2.f - w / 2.f;
    iconBounds = HUDRect(x,y,w,w);
    icon.setBounds(iconBounds);
    
    assignButton.setBounds(iconBounds);

    HUDView::setBounds(r);
}

void PlayArea::setup()
{
    HUDView::setup();
}

void PlayArea::draw()
{
    float fade = 0.f;
    RelativeTime diff = Time::getCurrentTime() - fadeStartTime;
    if (diff < RelativeTime::milliseconds(FADE_TIME))
        fade = 1.f - diff.inMilliseconds() / (float)FADE_TIME;

    GLfloat padOnColor[4] = { 1.f, 1.f, 1.f, fade};
    GLfloat padOffColor[4] = { 1.f, 1.f, 1.f, 1.f};

    GLuint onTextureID = SkinManager::instance().getSelectedSkin().getTexture("pad_on");
    GLuint offTextureID = SkinManager::instance().getSelectedSkin().getTexture("pad_off");

    GLint blendSrc;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    GLint blendDst;
    glGetIntegerv(GL_BLEND_DST, &blendDst);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, offTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), padOffColor, 0);
    defaultBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, onTextureID);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), padOnColor, 0);
    glLineWidth(1.f);
    defaultBatch.Draw();

    int iconTextureId = 0;
    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
    if (kitUuidString != "Default") {
        Uuid kitUuid(kitUuidString);
        iconTextureId = KitManager::GetInstance().GetItem(kitUuid)->GetSample(selectedMidiNote)->GetTexture(true);

    }
    else {
        iconTextureId = KitManager::GetInstance().GetItem(0)->GetSample(selectedMidiNote)->GetTexture(true);
    }
    
    icon.setDefaultColor(onColor);
    
    icon.setDefaultTexture(iconTextureId);    

    if (fade > 0.f)
    {
        float wobble = sinf(4*3.14159*(1-fade)) * fade;
        wobble *= iconBounds.w / 10.f;
        icon.setBounds(HUDRect(iconBounds.x - wobble, iconBounds.y-wobble, iconBounds.w+wobble*2, iconBounds.h+wobble*2));
    }
    
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    Environment::instance().modelViewMatrix.PushMatrix();
    Environment::instance().modelViewMatrix.Translate(getBounds().x, getBounds().y, 0.0f);
    Environment::instance().modelViewMatrix.Translate(getBounds().w / 2, getBounds().h / 2.f, 0.0f);
    Environment::instance().modelViewMatrix.Rotate(iconRotation * fade, 0.0f, 0.0f, 1.f);
    Environment::instance().modelViewMatrix.Translate(-(getBounds().w / 2.f), -(getBounds().h / 2.f), 0.0f);
    icon.draw();
    Environment::instance().modelViewMatrix.PopMatrix();
    
    glBlendFunc(blendSrc, blendDst);
    
    Environment::instance().modelViewMatrix.PushMatrix();
    Environment::instance().modelViewMatrix.Translate(getBounds().x, getBounds().y, 0.0f);
    assignButton.draw();
    Environment::instance().modelViewMatrix.PopMatrix();
}

void PlayArea::tap(int midiNote)
{
    if (midiNote == selectedMidiNote) {
        fadeStartTime = Time::getCurrentTime();
        iconRotation = (Random::getSystemRandom().nextFloat() * 2.f - 1.f) * 5;
    }
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

void PlayArea::enableAssignButton(bool shouldBeEnabled)
{
    assignButton.setVisible(shouldBeEnabled);
    assignButton.setEnabled(shouldBeEnabled);
}

void PlayArea::buttonStateChanged(HUDButton* b)
{
    if (b == &assignButton)
        for (Listener* l : listeners)
            l->playAreaChanged(this);
}

void PlayArea::addListener(PlayArea::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter == listeners.end())
        listeners.push_back(listener);
}

void PlayArea::removeListener(PlayArea::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter != listeners.end())
        listeners.erase(iter);
}

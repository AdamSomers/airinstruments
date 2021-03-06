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
    addChild(&text);
    
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
    
    addChild(&assignButton);
    assignButton.setText(StringArray("Assign"), StringArray("Assign"));
    assignButton.setVisible(false, 0);
    assignButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    assignButton.addListener(this);
    
    addChild(&clearButton);
    clearButton.setText(StringArray("Clear"), StringArray("Clear"));
    clearButton.setVisible(false, 0);
    clearButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    clearButton.addListener(this);
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
    TextureDescription td = text.getDefaultTexture();
    float aspectRatio = (float)td.imageW / (float)td.imageH;
    float textHeight = jmin(r.w, r.h) / 10.f;
    float textWidth = textHeight * aspectRatio;
    text.setBounds(HUDRect(r.w / 2.f - textWidth / 2.f,
                           jmax(0.f, y - textHeight - 20),
                           textWidth, textHeight));
    
    assignButton.setBounds(iconBounds);
    clearButton.setBounds(iconBounds);

    HUDView::setBounds(r);
}

void PlayArea::setup()
{
    HUDView::setup();
}

void PlayArea::draw()
{
    if (needsSetup) {
        setBounds(getBounds());
        needsSetup = false;
    }

    float fade = 0.f;
    RelativeTime diff = Time::getCurrentTime() - fadeStartTime;
    if (diff < RelativeTime::milliseconds(FADE_TIME))
        fade = 1.f - diff.inMilliseconds() / (float)FADE_TIME;

    GLfloat padOnColor[4] = { 1.f, 1.f, 1.f, fade};
    GLfloat padOffColor[4] = { 1.f, 1.f, 1.f, 1.f};

    TextureDescription onTextureDesc = SkinManager::instance().getSelectedSkin().getTexture("pad_on");
    TextureDescription offTextureDesc = SkinManager::instance().getSelectedSkin().getTexture("pad_off");

    GLint blendSrc;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    GLint blendDst;
    glGetIntegerv(GL_BLEND_DST, &blendDst);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, offTextureDesc.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), padOffColor, 0);
    defaultBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, onTextureDesc.textureId);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), padOnColor, 0);
    glLineWidth(1.f);
    defaultBatch.Draw();
    
    icon.setDefaultColor(onColor);    

    if (fade > 0.f)
    {
        float wobble = sinf(4*3.14159f*(1-fade)) * fade;
        wobble *= iconBounds.w / 10.f;
        icon.setBounds(HUDRect(iconBounds.x - wobble, iconBounds.y-wobble, iconBounds.w+wobble*2, iconBounds.h+wobble*2));
    }
    
    icon.setDefaultBlendMode(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
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
    clearButton.draw();
    text.draw();
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

    TextureDescription iconTextureDesc;
    TextureDescription categoryTextureDesc;
    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
    if (kitUuidString != "Default") {
        Uuid kitUuid(kitUuidString);
        iconTextureDesc = KitManager::GetInstance().GetItem(kitUuid)->GetSample(selectedMidiNote)->GetTexture(true);
        String category = KitManager::GetInstance().GetItem(kitUuid)->GetSample(selectedMidiNote)->GetCategory();
        categoryTextureDesc = SkinManager::instance().getSelectedSkin().getTexture("Text_" + category);
        
    }
    else {
        iconTextureDesc = KitManager::GetInstance().GetItem(0)->GetSample(selectedMidiNote)->GetTexture(true);
        String category = KitManager::GetInstance().GetItem(0)->GetSample(selectedMidiNote)->GetCategory();
        categoryTextureDesc = SkinManager::instance().getSelectedSkin().getTexture("Text_" + category);
    }
    
    icon.setDefaultTexture(iconTextureDesc);
    text.setDefaultTexture(categoryTextureDesc);

    needsSetup = true;
}

void PlayArea::loadTextures()
{
}

void PlayArea::setVisible(bool shouldBeVisible, int fadeTimeMs)
{
    HUDView::setVisible(shouldBeVisible, fadeTimeMs);
    assignButton.setVisible(false, 0);
    clearButton.setVisible(false, 0);
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

void PlayArea::enableClearButton(bool shouldBeEnabled)
{
    

    clearButton.setVisible(shouldBeEnabled);
    clearButton.setEnabled(shouldBeEnabled);
}

void PlayArea::buttonStateChanged(HUDButton* b)
{
    if (b == &assignButton)
        sendActionMessage("assign/" + String(id));
    else if (b == &clearButton)
        sendActionMessage("clear/" + String(id));
}

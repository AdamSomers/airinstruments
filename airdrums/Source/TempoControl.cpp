//
//  TempoControl.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/16/13.
//
//

#include "TempoControl.h"
#include "GfxTools.h"
#include "Main.h"
#include "Drums.h"

TempoControl::TempoControl()
: needsLayout(false)
, trackedFinger(nullptr)
{
    for (int i = 0; i < 5; ++i) {
        Icon* icon = new Icon;
        icons.push_back(icon);
        addChild(icon);
    }
    
    addChild(&leftButton);
    addChild(&rightButton);
    leftButton.addListener(this);
    rightButton.addListener(this);

    leftButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_left"), SkinManager::instance().getSelectedSkin().getTexture("arrow_left"));
    rightButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_right"), SkinManager::instance().getSelectedSkin().getTexture("arrow_right"));
    
    
    leftButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    rightButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    
    leftButton.setTimeout(300);
    rightButton.setTimeout(300);
}

TempoControl::~TempoControl()
{
    for (Icon* i : icons)
        delete i;
}

void TempoControl::draw()
{
    GLuint textureId = SkinManager::instance().getSelectedSkin().getTexture("tempo_background");
    setDefaultTexture(textureId);
    if (needsLayout) {
        layoutIcons();
        needsLayout = false;
    }
    HUDView::draw();
}

void TempoControl::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
    layoutIcons();
    
    float buttonWidth = b.h;
    leftButton.setBounds(HUDRect(-buttonWidth - 10,
                                 0,
                                 buttonWidth,
                                 buttonWidth));
    rightButton.setBounds(HUDRect(b.w + 10,
                                  0,
                                  buttonWidth,
                                  buttonWidth));
}

void TempoControl::layoutIcons()
{
    float iconWidth = getBounds().w / 3.f;
    float iconHeight = getBounds().h;
    
    bool movingLeft = icons.at(1)->getBounds().x > 0;
    if (movingLeft)
        icons.at(0)->setBounds(HUDRect(-iconWidth, 0, iconWidth, iconHeight));
    else
        icons.at(0)->setBoundsImmediately(HUDRect(-iconWidth, 0, iconWidth, iconHeight));
    icons.at(1)->setBounds(HUDRect(0, 0, iconWidth, iconHeight));
    icons.at(2)->setBounds(HUDRect(getBounds().w / 2.f - iconWidth / 2, 0, iconWidth, iconHeight));
    icons.at(3)->setBounds(HUDRect(getBounds().w - iconWidth, 0, iconWidth, iconHeight));
    if (!movingLeft)
        icons.at(4)->setBounds(HUDRect(getBounds().w, 0, iconWidth, iconHeight));
    else
        icons.at(4)->setBoundsImmediately(HUDRect(getBounds().w, 0, iconWidth, iconHeight));
}

void TempoControl::setTempo(float tempo)
{
    icons.at(0)->setTempo(tempo - 2);
    icons.at(1)->setTempo(tempo - 1);
    icons.at(2)->setTempo(tempo);
    icons.at(3)->setTempo(tempo + 1);
    icons.at(4)->setTempo(tempo + 2);
}

void TempoControl::increment(int direction)
{
    if (direction < 0)
    {
        Icon* front = icons.front();
        front->setTempo(icons.at(2)->getTempo() + 3);
        icons.pop_front();
        icons.push_back(front);
    }
    else
    {
        Icon* back = icons.back();
        back->setTempo(icons.at(2)->getTempo() - 3);
        icons.pop_back();
        icons.push_front(back);
    }
    
    Drums::instance().setTempo(icons.at(2)->getTempo());
    needsLayout = true;
}

void TempoControl::enableButtons(bool shouldBeEnabled)
{
    leftButton.setVisible(shouldBeEnabled);
    rightButton.setVisible(shouldBeEnabled);
}

void TempoControl::fingerMotion(float /*x*/, float /*y*/, FingerView* /*fv*/)
{
#if 0
    if (fv != trackedFinger)
        return;

    stopTimer(kTimerTrackedFingerMissing);
    startTimer(kTimerTrackedFingerMissing, 100);
    
    int inc = 0;
    float center = getBounds().x + getBounds().w / 2;
    float distanceFromCenter = x - center;
    if (fabsf(distanceFromCenter) > 15 && x < center)
        inc = 1;
    else if (fabsf(distanceFromCenter) > 15)
        inc = -1;
    
    if (inc != 0 && !isTimerRunning(kTimerSelectionDelay)) {
        increment(inc);
        float multiplier = fabsf(distanceFromCenter*2.f) / getBounds().w;
        startTimer(kTimerSelectionDelay, (int) jmax<float>(100.0f, 500.0f * (1.f-multiplier)));
    }
#endif
}

void TempoControl::fingerEntered(float /*x*/, float /*y*/, FingerView* /*fv*/)
{
#if 0
    M3DVector3f vec;
    fv->objectFrame.GetOrigin(vec);
    //printf("%x entered %d %d %.2f %.2f\n", fv, (int)x, (int)y, vec[0], vec[1]);
    
    if (!trackedFinger)
        trackedFinger = fv;
    
    if (fv == trackedFinger)
    {
        //Logger::outputDebugString(String(x));
        initialFingerX = x;
        initialFingerY = y;
    }
#endif
}

void TempoControl::fingerExited(float /*x*/, float /*y*/, FingerView* /*fv*/)
{
#if 0
    M3DVector3f vec;
    fv->objectFrame.GetOrigin(vec);
    //printf("%x exited %d %d %.2f %.2f\n", fv, (int)x, (int)y, vec[0], vec[1]);
    if (fv == trackedFinger)
    {
        trackedFinger = NULL;
        initialFingerX = x;
        initialFingerY = y;
    }
#endif
}

void TempoControl::cursorMoved(float /*x*/, float /*y*/)
{
#if 0
    int inc = 0;
    float center = getBounds().x + getBounds().w / 2;
    float distanceFromCenter = x - center;
    if (fabsf(distanceFromCenter) > 15 && x < center)
        inc = 1;
    else if (fabsf(distanceFromCenter) > 15)
        inc = -1;
    
    if (inc != 0 && !isTimerRunning(kTimerSelectionDelay)) {
        increment(inc);
        float multiplier = fabsf(distanceFromCenter*2.f) / getBounds().w;
        startTimer(kTimerSelectionDelay, (int) jmax<float>(100.0f, 500.0f * (1.f-multiplier)));
    }
#endif
}

void TempoControl::cursorEntered(float /*x*/, float /*y*/)
{
}

void TempoControl::cursorExited(float, float)
{
}


void TempoControl::timerCallback(int timerId)
{
    switch (timerId)
    {
    case kTimerSelectionDelay:
        stopTimer(kTimerSelectionDelay);
        break;

    case kTimerTrackedFingerMissing:
        trackedFinger = NULL;
        break;
    }
}

void TempoControl::buttonStateChanged(HUDButton* b)
{
    if (b == &leftButton)
        increment(1);
    if (b == &rightButton)
        increment(-1);
}

TempoControl::Icon::Icon()
: textureId (0)
, tempoValue(-999)
, tempoValueChanged(true)
{
}

void TempoControl::Icon::draw()
{
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
        fabsf(tempBounds.y - targetBounds.y) > 2 ||
        fabsf(tempBounds.w - targetBounds.w) > 2 ||
        fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);


    if (tempoValueChanged) // need to generate a new texture
    {
        Image im(Image::PixelFormat::ARGB, (int) (targetBounds.w*4), (int) (targetBounds.h*4), true);
        Graphics g (im);
        g.setColour(Colours::white);
        g.setFont((targetBounds.h * .75f) * 4.0f);
        g.drawText(String((int)tempoValue), 0, 0, (int) (targetBounds.w*4), (int) (targetBounds.h*4), Justification::centred, true);
        if (textureId != 0)
            glDeleteTextures(1, &textureId);
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        GfxTools::loadTextureFromJuceImage(im);
        setDefaultTexture(textureId);
        tempoValueChanged = false;
    }
    
    if (getBounds().x + getBounds().w * .33 > 0 && getBounds().x + getBounds().w * .66 < getParent()->getBounds().w)
        HUDView::draw();
}

void TempoControl::Icon::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(b);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
}

void TempoControl::Icon::setBoundsImmediately(const HUDRect &b)
{
    tempBounds = b;
    targetBounds = b;
    HUDView::setBounds(b);
}

void TempoControl::Icon::updateBounds()
{
    if (fabsf(tempBounds.x - targetBounds.x) < 2)
        tempBounds.x = targetBounds.x;
    else
        tempBounds.x += xStep;
    
    if (fabsf(tempBounds.y - targetBounds.y) < 2)
        tempBounds.y = targetBounds.y;
    else
        tempBounds.y += yStep;
    
    if (fabsf(tempBounds.w - targetBounds.w) < 2)
        tempBounds.w = targetBounds.w;
    else
        tempBounds.w += wStep;
    
    if (fabsf(tempBounds.h - targetBounds.h) < 2)
        tempBounds.h = targetBounds.h;
    else
        tempBounds.h += hStep;
    
    HUDView::setBounds(tempBounds);
}

void TempoControl::Icon::setTempo(float tempo)
{
    tempoValue = tempo;
    tempoValueChanged = true;
}

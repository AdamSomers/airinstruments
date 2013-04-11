//
//  KitSelector.cpp
//  AirBeats
//
//  Created by Adam Somers on 4/9/13.
//
//

#include "KitSelector.h"
#include "SkinManager.h"
#include "KitManager.h"
#include "Main.h"

KitSelector::KitSelector()
: selection(0)
, needsLayout(false)
, trackedFinger(nullptr)
, enabled(false)
{
    setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("wheelBg"));
    int numKits = KitManager::GetInstance().GetItemCount();
    for (int i = 0; i < numKits; ++i)
    {
        Icon* icon = new Icon(i);
        icons.push_back(icon);
        addChild(icon);
    }
}

KitSelector::~KitSelector()
{
    
}

void KitSelector::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(tempBounds);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
    
    for (Icon* i : icons)
    {
        float aspectRatio = 1.0f;
        String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("selectedKitUuid", "Default");
        if (kitUuidString != "Default") {
            Uuid kitUuid(kitUuidString);
            Image image = KitManager::GetInstance().GetItem(kitUuid)->GetImage();
            if (image.isValid())
                aspectRatio = image.getWidth() / (float)image.getHeight();
        }
        else {
            const Image& image = KitManager::GetInstance().GetItem(0)->GetImage();
            aspectRatio = image.getWidth() / (float)image.getHeight();
        }
        
        float height = (float)(b.w/2) / aspectRatio;
        i->setBounds(HUDRect(0,
                             0,
                             b.w/2,
                             (int) height));
    }
    
    layoutIcons();
}

void KitSelector::updateBounds()
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

void KitSelector::layoutIcons()
{
    int N = icons.size();

    
    int first = selection;
    int i = first;
    for (int count = 0; count < N; ++count)
    {        
        Icon* icon = icons.at(i);
        icon->setRotationCoefficient((count/(float)N * 360.f));
        i = (i+1) % N;
    }
}

void KitSelector::draw()
{
    if (needsLayout) {
        layoutIcons();
        needsLayout = false;
    }
    
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
    fabsf(tempBounds.y - targetBounds.y) > 2 ||
    fabsf(tempBounds.w - targetBounds.w) > 2 ||
    fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);
    
    HUDView::draw();
}

void KitSelector::setSelection(int sel)
{
    float direction = sel > selection ? -1.f : 1.f;
    if (sel < 0) sel = icons.size() - 1;
    if (sel >= (int) icons.size()) sel = 0;
    selection = sel;
    needsLayout = true;
}

void KitSelector::incSelection(int direction)
{
    selection += direction;
    if (selection < 0) selection = icons.size() - 1;
    if (selection >= (int) icons.size()) selection = 0;
    
    for (Icon* icon : icons)
        icon->rotate(-direction * 360.f / icons.size());
}

void KitSelector::fingerMotion(float x, float y, FingerView* fv)
{
    if (!enabled)
        return;

    if (fv != trackedFinger)
        return;
    
    int inc = 0;
    if (y - prevFingerY < -3)
        inc = 1;
    else if (y - prevFingerY > 3)
        inc = -1;
    
    if (inc != 0 && !isTimerRunning()) {
        incSelection(inc);
        for (Listener* l : listeners)
            l->kitSelectorChanged(this);
        startTimer(250);
        trackedFinger = nullptr;
    }
    
    prevFingerX = x;
    prevFingerY = y;
}

void KitSelector::fingerEntered(float x, float y, FingerView* fv)
{
    if (!trackedFinger)
        trackedFinger = fv;
    
    if (fv == trackedFinger)
    {
        prevFingerX = x;
        prevFingerY = y;
    }
}

void KitSelector::fingerExited(float x, float y, FingerView* fv)
{
    if (fv == trackedFinger)
    {
        trackedFinger = NULL;
        prevFingerX = x;
        prevFingerY = y;
    }
}

void KitSelector::timerCallback()
{
    stopTimer();
}

void KitSelector::addListener(KitSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter == listeners.end())
        listeners.push_back(listener);
}

void KitSelector::removeListener(KitSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter != listeners.end())
        listeners.erase(iter);
}

KitSelector::Icon::Icon(int inId)
: id(inId)
, rotationCoeff(0.f)
, targetRotationCoeff(0.f)
, rotationInc(0.f)
{
}

KitSelector::Icon::~Icon()
{
}

void KitSelector::Icon::draw()
{
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
        fabsf(tempBounds.y - targetBounds.y) > 2 ||
        fabsf(tempBounds.w - targetBounds.w) > 2 ||
        fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);
    
    if (fabsf(rotationCoeff - targetRotationCoeff) > 10.f) {
        rotationCoeff += rotationInc;
    }
    else if (rotationCoeff != targetRotationCoeff) {
        rotationCoeff = targetRotationCoeff;
        rotationInc = 0;
    }
    else
        rotationInc = 0;
    
    // sanity check
    if (fabsf(rotationCoeff) >= 360.f)
        rotationCoeff = targetRotationCoeff = 0.f;
    
    GLuint textureID = KitManager::GetInstance().GetItem(id)->GetTexture();
    
    setDefaultTexture(textureID);
    Environment::instance().modelViewMatrix.PushMatrix();
    Environment::instance().modelViewMatrix.Translate(getParent()->getBounds().w / 2, getParent()->getBounds().h / 2, 0);
    Environment::instance().modelViewMatrix.Rotate(rotationCoeff, 0, 0, 1);
    Environment::instance().modelViewMatrix.Translate(50,0,0);
    HUDView::draw();
    Environment::instance().modelViewMatrix.PopMatrix();
}

void KitSelector::Icon::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(b);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
}

void KitSelector::Icon::setRotationCoefficient(float rotation)
{
    rotationCoeff = rotation;
}

void KitSelector::Icon::rotate(float rotation)
{
    targetRotationCoeff = rotationCoeff + rotation;
    rotationInc = rotation / 10.f;
}

void KitSelector::Icon::updateBounds()
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

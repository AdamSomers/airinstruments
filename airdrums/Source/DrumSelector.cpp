#include "DrumSelector.h"
#include "Drums.h"
#include "GfxTools.h"
#include "KitManager.h"
#include "SkinManager.h"
#include "Main.h"

DrumSelector::DrumSelector()
: selection(0)
, needsLayout(false)
, trackedFinger(nullptr)
{
    for (int i = 0; i < 16; ++i)
    {
        Icon* icon = new Icon(i);
        icons.push_back(icon);
        addChild(icon);
    }
}

DrumSelector::~DrumSelector()
{
}

void DrumSelector::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    layoutIcons();
}

void DrumSelector::layoutIcons()
{
    int N = icons.size();

    const int selectedIconWidth = (int) getBounds().h;
    const int selectedIconHeight = (int) getBounds().h;
    const float iconWidth = (getBounds().w-selectedIconWidth) / (float)(N - (N%2));
    const float iconHeight = iconWidth;

    int first = (selection + N / 2) % N;
    int i = first;
    for (int count = 0; count < N; ++count)
    {
        
        Icon* icon = icons.at(i);
        if (count < N/2)
            icon->setBounds(HUDRect(iconWidth*count + 10, getBounds().h / 2 - iconHeight / 2, iconWidth, iconHeight));
        else if (count > N/2)
            icon->setBounds(HUDRect(iconWidth*(count-1) + selectedIconWidth + 10, getBounds().h / 2 - iconHeight / 2, iconWidth, iconHeight));
        else
            icon->setBounds(HUDRect(getBounds().w / 2 - selectedIconWidth  / 2 + 10, getBounds().h / 2 - selectedIconHeight / 2, (GLfloat) selectedIconWidth, (GLfloat) selectedIconHeight));
        i = (i+1) % N;
    }
}

void DrumSelector::setup()
{  
    HUDView::setup();
}

void DrumSelector::draw()
{
    if (needsLayout) {
        layoutIcons();
        needsLayout = false;
    }

    //GLuint textureId = SkinManager::instance().getSelectedSkin().getTexture("DrumSelectorBg");
    //setDefaultTexture(textureId);
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
    HUDView::draw();
}


void DrumSelector::loadTextures()
{
}

void DrumSelector::setSelection(int sel)
{
    if (sel < 0) sel = icons.size() - 1;
    if (sel >= (int) icons.size()) sel = 0;
    selection = sel;
    needsLayout = true;
}

void DrumSelector::fingerMotion(float x, float /*y*/, FingerView* fv)
{
    if (fv != trackedFinger)
        return;

    int inc = 0;
    float center = getBounds().x + getBounds().w / 2;
    float distanceFromCenter = x - center;
    if (fabsf(distanceFromCenter) > 30 && x < center)
        inc = -1;
    else if (fabsf(distanceFromCenter) > 30)
        inc = 1;

    if (inc != 0 && !isTimerRunning()) {
        setSelection(selection + inc);
        for (Listener* l : listeners)
            l->drumSelectorChanged(this);
        float multiplier = fabsf(distanceFromCenter*2.f) / getBounds().w;
        startTimer((int) jmax<float>(100.0f, 500.0f * (1.f-multiplier)));
    }
}

void DrumSelector::fingerEntered(float /*x*/, float /*y*/, FingerView* fv)
{    
    M3DVector3f vec;
    fv->objectFrame.GetOrigin(vec);
    //printf("%x entered %d %d %.2f %.2f\n", fv, (int)x, (int)y, vec[0], vec[1]);
    
    if (!trackedFinger) {
        trackedFinger = fv;
        startTimer(500);
    }
}

void DrumSelector::fingerExited(float x, float /*y*/, FingerView* fv)
{
    // !!! Hack to get avoid spurious exits due to corrupt screen coord data
    if (x < 0 || x > 2000)
        return;
    
    M3DVector3f vec;
    fv->objectFrame.GetOrigin(vec);
    //printf("%x exited %d %d %.2f %.2f\n", fv, (int)x, (int)y, vec[0], vec[1]);
    if (fv == trackedFinger)
    {
        trackedFinger = NULL;
    }
}

void DrumSelector::timerCallback()
{
    stopTimer();
}

void DrumSelector::addListener(DrumSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter == listeners.end())
        listeners.push_back(listener);
}

void DrumSelector::removeListener(DrumSelector::Listener *listener)
{
    auto iter = std::find(listeners.begin(), listeners.end(), listener);
    if (iter != listeners.end())
        listeners.erase(iter);
}

DrumSelector::Icon::Icon(int inId)
: id(inId)
{
}

DrumSelector::Icon::~Icon()
{
}

void DrumSelector::Icon::setup()
{
    HUDView::setup();
}

void DrumSelector::Icon::draw()
{
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
        fabsf(tempBounds.y - targetBounds.y) > 2 ||
        fabsf(tempBounds.w - targetBounds.w) > 2 ||
        fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDView::setBounds(targetBounds);

    GLuint textureID = 0;
    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
    if (kitUuidString != "Default") {
        Uuid kitUuid(kitUuidString);
        textureID = KitManager::GetInstance().GetItem(kitUuid)->GetSample(id)->GetTexture();
    }
    else
        textureID = KitManager::GetInstance().GetItem(0)->GetSample(id)->GetTexture();
    
    setDefaultTexture(textureID);
    HUDView::draw();
}

void DrumSelector::Icon::setBounds(const HUDRect &b)
{
    tempBounds = getBounds();
    targetBounds = b;
    
    HUDView::setBounds(b);
    
    xStep = (targetBounds.x - tempBounds.x) / 10.f;
    yStep = (targetBounds.y - tempBounds.y) / 10.f;
    wStep = (targetBounds.w - tempBounds.w) / 10.f;
    hStep = (targetBounds.h - tempBounds.h) / 10.f;
}

void DrumSelector::Icon::updateBounds()
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

void DrumSelector::Icon::loadTextures()
{
}
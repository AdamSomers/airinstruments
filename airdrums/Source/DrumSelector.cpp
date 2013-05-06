#include "DrumSelector.h"
#include "Drums.h"
#include "GfxTools.h"
#include "KitManager.h"
#include "SkinManager.h"
#include "Main.h"

DrumSelector::DrumSelector()
: needsLayout(false)
, selectedItem(-1)
{
    for (int i = 0; i < 16; ++i)
    {
        SharedPtr<Icon> icon(new Icon(i));
        icons.push_back(icon);
        addChild(icon.get());
        icon->addListener(this);
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
    const int N = icons.size();

    const float minIconSpacing = 15.f;
    float iconSpacing = minIconSpacing;
    if ((getBounds().h + minIconSpacing) * N < getBounds().w)
        iconSpacing = (getBounds().w - (getBounds().h * N)) / (N+2);
    const float availableWidth = getBounds().w - iconSpacing * (float)(N+2);
    const float iconWidth = jmin(availableWidth / (float)N, getBounds().h);
    const float iconHeight = iconWidth;
    
    float iconX = iconSpacing;
    for (int i = 0; i < N; ++i)
    {
        SharedPtr<Icon> icon = icons.at(i);
        icon->setBounds(HUDRect(iconX, getBounds().h / 2 - iconHeight / 2, iconWidth, iconHeight));
        iconX += iconWidth + iconSpacing;
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

void DrumSelector::setPadAssociation(int note, int pad)
{
    if (note < 0) note = icons.size() - 1;
    if (note >= (int) icons.size()) note = 0;

    for (SharedPtr<Icon> i : icons)
        if (i->getPadNumber() == pad)
            i->setPadNumber(-1);

    icons.at(note)->setPadNumber(pad);

    needsLayout = true;
}

int DrumSelector::getPadForNote(int note) const
{
    jassert(note >= 0 && note < icons.size())
    return icons.at(note)->getPadNumber();
}

int DrumSelector::getNoteForPad(int pad) const
{
    int note = -1;
    jassert(pad >= 0 && pad < 6)
    for (int i = 0; i < icons.size(); ++i)
        if (getPadForNote(i) == pad)
            note = i;
    return note;
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

void DrumSelector::buttonStateChanged(HUDButton* b)
{
    setSelection(b->getId());
    for (Listener* l : listeners)
        l->drumSelectorChanged(b->getId());
}

void DrumSelector::setSelection(int selection)
{
    for (SharedPtr<Icon> i : icons) {
        i->setHighlighted(i->getId() == selection);
        if (selection != i->getId())
            i->setState(i->getPadNumber() != -1);
    }
    
    selectedItem = selection;
}

DrumSelector::Icon::Icon(int inId)
: HUDButton(inId)
, padNumber(-1)
, highlighted(false)
, flashState(false)
{
    setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
}

DrumSelector::Icon::~Icon()
{
}

void DrumSelector::Icon::draw()
{
    if (fabsf(tempBounds.x - targetBounds.x) > 2 ||
        fabsf(tempBounds.y - targetBounds.y) > 2 ||
        fabsf(tempBounds.w - targetBounds.w) > 2 ||
        fabsf(tempBounds.h - targetBounds.h) > 2)
        updateBounds();
    else if (tempBounds != targetBounds)
        HUDButton::setBounds(targetBounds);

    GLuint onTextureID = 0;
    GLuint offTextureID = 0;
    
    if (padNumber != -1)
    {
        String padColor = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("padColor" + String(padNumber), "FFFFFF");
        Colour c = Colour::fromString(padColor);
        GLfloat color[4] = { c.getFloatRed(), c.getFloatGreen(), c.getFloatBlue(), c.getFloatAlpha() };
        setOnColor(color);
    }
    
    if (highlighted)
    {
        RelativeTime diff = Time::getCurrentTime() - flashStart;
        if (diff.inMilliseconds() >= 300)
        {
            flashState = !flashState;
            flashStart = Time::getCurrentTime();
        }

        GLfloat color[4] = { flashState * 1.f, flashState * 1.f, flashState * 1.f, 1.f };
        setOnColor(color);
    }

    String kitUuidString = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getValue("kitUuid", "Default");
    if (kitUuidString != "Default") {
        Uuid kitUuid(kitUuidString);
        offTextureID = KitManager::GetInstance().GetItem(kitUuid)->GetSample(getId())->GetTexture(false);
        onTextureID = KitManager::GetInstance().GetItem(kitUuid)->GetSample(getId())->GetTexture(true);
        setTextures(onTextureID, offTextureID);
    }
    else {
        offTextureID = KitManager::GetInstance().GetItem(0)->GetSample(getId())->GetTexture(false);
        onTextureID = KitManager::GetInstance().GetItem(0)->GetSample(getId())->GetTexture(true);
        setTextures(onTextureID, offTextureID);
    }
    
    HUDButton::draw();
}

void DrumSelector::Icon::setPadNumber(int pad)
{
    padNumber = pad;
    setState(padNumber != -1);
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

void DrumSelector::Icon::setHighlighted(bool shouldBeHighlighted)
{
    highlighted = shouldBeHighlighted;
    if (shouldBeHighlighted) {
        flashStart = Time::getCurrentTime();
        setState(true);
    }
}

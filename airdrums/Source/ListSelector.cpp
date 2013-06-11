//
//  ListSelector.cpp
//  AirBeats
//
//  Created by Adam Somers on 6/5/13.
//
//

#include "ListSelector.h"
#include "SkinManager.h"
#include "Main.h"

static const float sIconSpacing = 15.f;

ListSelector::ListSelector(String name, bool left)
: selection(0)
, needsLayout(false)
, shownX(0.f)
, hiddenX(0.f)
, leftHanded(left)
, enabled(false)
, iconHeight(0.f)
, maxVisibleIcons(20)
{
    //setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("listSelectorBg"));
    
    displayToggleButton.addListener(this);
    displayToggleButton.setText(StringArray(name), StringArray(name));
    displayToggleButton.setTextColor(Colour::fromFloatRGBA(0.f,0.f,0.f,1.f));
    displayToggleButton.setBackgroundColor(Colour::fromFloatRGBA(0.f,0.f,0.f,0.f));
    upButton.addListener(this);
    downButton.addListener(this);

    addChild(&tabView);
    if (leftHanded)
        tabView.setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("tableft"));
    else
        tabView.setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("tabright"));
    
    addChild(&displayToggleButton);
    addChild(&upButton);
    addChild(&downButton);
    
    setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("drawer"));
    displayToggleButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    upButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    downButton.setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
    if (leftHanded)
        displayToggleButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_left"),
                                        SkinManager::instance().getSelectedSkin().getTexture("arrow_right"));
    else
        displayToggleButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_right"),
                                        SkinManager::instance().getSelectedSkin().getTexture("arrow_left"));
    
    upButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_up_white"),
                         SkinManager::instance().getSelectedSkin().getTexture("arrow_up_white"));
    downButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_down_white"),
                           SkinManager::instance().getSelectedSkin().getTexture("arrow_down_white"));

    upButton.setVisible(false);
    upButton.setButtonType(HUDButton::kMomentary);
    upButton.setTimeout(250);
    downButton.setVisible(false);
    downButton.setButtonType(HUDButton::kMomentary);
    downButton.setTimeout(250);
}

ListSelector::~ListSelector()
{
    for (Icon* i : icons)
    {
        delete i;
    }
}

void ListSelector::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
    
    float tabHeight = b.h/2.f;
    float tabWidth = tabHeight/2.f;
    float tabX = leftHanded ? b.w : -tabWidth;
    
    tabView.setBounds(HUDRect(tabX, b.h / 2.f - tabHeight / 2.f, tabWidth, tabHeight));
    
    float buttonWidth = tabWidth;
    float buttonHeight = tabWidth;
    float buttonX = leftHanded ? b.w : -buttonWidth;
    
    displayToggleButton.setBounds(HUDRect(buttonX,
                                          b.h / 2.f - buttonHeight / 2.f,
                                          buttonWidth,
                                          buttonHeight));

    buttonWidth = 35;
    buttonHeight = 35;

    upButton.setBounds(HUDRect(b.w / 2.f - buttonWidth / 2.f,
                               b.h-buttonHeight,
                               buttonWidth,
                               buttonHeight));
    
    downButton.setBounds(HUDRect(b.w / 2.f - buttonWidth / 2.f,
                                 0,                                 
                                 buttonWidth,
                                 buttonHeight));

    float maxIconWidth = getBounds().w - 10;
    iconHeight = 50.f;
    
    // if any item exceeds max width, need to re-adjust height for all
    for (int i = 0; i < (int) icons.size(); ++i)
    {
        Icon* icon = icons.at(i);
        const TextureDescription& td = icon->getDefaultTexture();
        float aspectRatio = (float)td.imageW / (float)td.imageH;
        float w = iconHeight * aspectRatio;
        if (w > maxIconWidth)
        {
            iconHeight = maxIconWidth / aspectRatio;
        }
    }
    
    iconHeight += sIconSpacing;
    maxVisibleIcons = jmax(1, (int)((b.h - buttonHeight * 2) / iconHeight));
    updateVisibleIcons();

    // setup the icons' size
    for (int i = 0; i < (int) icons.size(); ++i)
    {
        Icon* icon = icons.at(i);
        float aspectRatio = 1.0f;
        const TextureDescription& td = icon->getDefaultTexture();
        if (td.imageH != 0)
            aspectRatio = (float)td.imageW / (float)td.imageH;
        
        float width = b.w;
        float height = iconHeight;
        icon->setBounds(HUDRect(0,
                                0,
                                width,
                                height));
    }
    // place the icons in the correct positions
    layoutIcons();
}

void ListSelector::layoutIcons()
{
    int N = visibleIcons.size();
    
    HUDRect buttonRect(5,
                       upButton.getBounds().y - iconHeight - 5,
                       visibleIcons.at(0)->getBounds().w,
                       iconHeight);
    for (int i = 0; i < N; ++i)
    {
        buttonRect.w = visibleIcons.at(i)->getBounds().w;
        Icon* icon = visibleIcons.at(i);
        icon->setBounds(buttonRect);
        if (icon->getId() == getSelection()) {
            icon->isSelected = true;
        }
        else {
            icon->isSelected = false;
        }
        buttonRect.y -= iconHeight;
    }
}

void ListSelector::updateVisibleIcons()
{
    if (visibleIcons.size() == 0)
        return;
    int first = visibleIcons.front()->getId();
    if (first + maxVisibleIcons > (int)icons.size())
        first = jmax(0, first - ((first + maxVisibleIcons) - (int)icons.size()));
    int last = jmin(first + maxVisibleIcons, (int)icons.size());
    for (auto iter = visibleIcons.begin(); iter != visibleIcons.end(); ++iter)
        removeChild(*iter);
    visibleIcons.clear();
    for (int i = first; i < last; ++i)
    {
        visibleIcons.push_back(icons.at(i));
        addChild(icons.at(i));
    }
    if (icons.size() > maxVisibleIcons) {
        upButton.setVisible(true);
        downButton.setVisible(true);
    }
    else {
        upButton.setVisible(false);
        downButton.setVisible(false);
    }
        
}

void ListSelector::updateBounds()
{
    int elapsed = (Time::getCurrentTime() - displayChangeStart).inMilliseconds();
    float progress = elapsed / 500.f;
    float ease = 0.5f * sinf(3.14159f*progress - 3.14159f/2.f) + 0.5f;
    float x = 0.f;
    float distance = hiddenX - shownX;
    if (enabled) {
        x = shownX + distance * (1.f - ease);
        if (progress >= 1.f && x != shownX)
            x = shownX;
    }
    else {
        x = shownX + distance * ease;
        if (progress >= 1.f && x != hiddenX)
            x = hiddenX;
    }
    
    HUDView::setBounds(HUDRect(x, getBounds().y, getBounds().w, getBounds().h));
}

void ListSelector::addIcon(Icon *icon)
{
    icons.push_back(icon);
    if (visibleIcons.size() < maxVisibleIcons) {
        visibleIcons.push_back(icon);
        addChild(icon);
    }

    if (icons.size() > maxVisibleIcons) {
        upButton.setVisible(true);
        downButton.setVisible(true);
    }
}

void ListSelector::removeAllIcons()
{
    for (Icon* i : icons)
    {
        if (i->getDefaultTexture().textureId != 0)
            glDeleteTextures(1, &(i->getDefaultTexture().textureId));
        removeChild(i);
        delete i;
    }
    icons.clear();
    visibleIcons.clear();
}

void ListSelector::draw()
{
    if (needsLayout) {
        layoutIcons();
        needsLayout = false;
    }
    
    if ((getBounds().x != shownX && enabled) || (getBounds().x != hiddenX && !enabled))
        updateBounds();
    
    HUDView::draw();
}

void ListSelector::setSelection(int sel)
{
    if (sel < 0) sel = icons.size() - 1;
    if (sel >= (int) icons.size()) sel = 0;
    selection = sel;
    
    int first = visibleIcons.front()->getId();
    int last = visibleIcons.back()->getId();
    if (icons.size() > visibleIcons.size() &&
        (selection < first ||
         selection > last))
    {
        for (auto iter = visibleIcons.begin(); iter != visibleIcons.end(); ++iter)
            removeChild(*iter);
        visibleIcons.clear();
        if (selection < first)
        {
            jassert(selection + maxVisibleIcons < icons.size());
            for (int i = selection; i < selection + maxVisibleIcons; ++i)
            {
                visibleIcons.push_back(icons.at(i));
                addChild(icons.at(i));
            }
        }
        else if (selection > last)
        {
            int start = selection - maxVisibleIcons;
            jassert(start + maxVisibleIcons < icons.size());
            for (int i = start; i < start + maxVisibleIcons; ++i)
            {
                visibleIcons.push_back(icons.at(i));
                addChild(icons.at(i));
            }
        }
    }
    
    needsLayout = true;
}

void ListSelector::buttonStateChanged(HUDButton *b)
{
    if (b == &displayToggleButton)
    {
        setEnabled(b->getState());
    }
    else if (b == &upButton)
    {
        if (visibleIcons.front()->getId() > 0)
        {
            visibleIcons.push_front(icons.at(visibleIcons.front()->getId() - 1));
            addChild(visibleIcons.front());
            removeChild(visibleIcons.back());
            visibleIcons.pop_back();
            needsLayout = true;
        }
    }
    else if (b == &downButton)
    {
        if (visibleIcons.back()->getId() < icons.size() - 1)
        {
            visibleIcons.push_back(icons.at(visibleIcons.back()->getId() + 1));
            addChild(visibleIcons.back());
            removeChild(visibleIcons.front());
            visibleIcons.pop_front();
            needsLayout = true;
        }
    }
    else
    {
        Icon* icon = dynamic_cast<Icon*>(b);
        if (icon)
        {
            setSelection(icon->getId());
            sendChangeMessage();
        }
    }
}

void ListSelector::setXRange(float shown, float hidden)
{
    shownX = shown;
    hiddenX = hidden;
}

void ListSelector::setEnabled(bool shouldBeEnabled)
{
    enabled = shouldBeEnabled;
    displayChangeStart = Time::getCurrentTime();
}


ListSelector::Icon::Icon(int inId)
: id(inId)
, isSelected(false)
{
    addChild(&highlightView);
    highlightView.setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("highlight"));
    highlightView.setMultiplyAlpha(true);
    highlightView.setVisible(false, 0);
    
    addChild(&imageView);
}

ListSelector::Icon::~Icon()
{
}

void ListSelector::Icon::draw()
{
    if (isTimerRunning())
    {
        RelativeTime elapsed = Time::getCurrentTime() - lastTimerStartTime;
        float progress = elapsed.inMilliseconds() / (float)hoverTimeout;
        highlightView.setVisible(true,0);
        highlightView.setBounds(HUDRect(-getBounds().x, 0, getParent()->getBounds().w * progress, getBounds().h));
    }
    else
    {
        highlightView.setVisible(isSelected,0);
        highlightView.setBounds(HUDRect(-getBounds().x, 0, (int)isSelected * getParent()->getBounds().w, getBounds().h));
    }
    
    HUDView::draw();
}

void ListSelector::Icon::setBounds(const HUDRect &b)
{
    HUDButton::setBounds(b);
    if (getParent())
        highlightView.setBounds(HUDRect(0,0,b.w,b.h));
    
    TextureDescription td = imageView.getDefaultTexture();
    float aspectRatio = td.imageW / (float)td.imageH;
    float h = b.h - sIconSpacing;
    float w = h * aspectRatio;
    w -= 10.f;
    h = w / aspectRatio;
    imageView.setBounds(HUDRect(5,sIconSpacing / 2.f,w,h));
}

void ListSelector::Icon::setTextures(TextureDescription on, TextureDescription off)
{
    imageView.setDefaultTexture(on);
}

void ListSelector::Icon::updateBounds()
{
}

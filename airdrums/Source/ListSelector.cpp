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

ListSelector::ListSelector(bool left)
: selection(0)
, needsLayout(false)
, shownX(0.f)
, hiddenX(0.f)
, leftHanded(left)
, enabled(false)
, iconSpacing(0.f)
, iconHeight(0.f)
{
    //setDefaultTexture(SkinManager::instance().getSelectedSkin().getTexture("listSelectorBg"));
    
    displayToggleButton.addListener(this);
    upButton.addListener(this);
    downButton.addListener(this);
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
    
    upButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_up"),
                         SkinManager::instance().getSelectedSkin().getTexture("arrow_up"));
    downButton.setTextures(SkinManager::instance().getSelectedSkin().getTexture("arrow_down"),
                           SkinManager::instance().getSelectedSkin().getTexture("arrow_down"));
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
    
    float buttonWidth = 75.f;
    float buttonHeight = 75.f;
    float x = leftHanded ? b.w + 10 : -buttonWidth - 10;
    displayToggleButton.setBounds(HUDRect(x,
                                          b.y + b.h / 2.f - buttonHeight / 2.f,
                                          buttonWidth,
                                          buttonHeight));
    upButton.setBounds(HUDRect(b.w / 2.f - buttonWidth / 2.f,
                               b.h-buttonHeight,
                               buttonWidth,
                               buttonHeight));
    
    downButton.setBounds(HUDRect(b.w / 2.f - buttonWidth / 2.f,
                                 0,                                 
                                 buttonWidth,
                                 buttonHeight));
    
    
    iconSpacing = 10.f;
    float availableHeight = b.h - buttonHeight * 2.f - iconSpacing * icons.size();
    float maxIconHeight = availableHeight / (float) icons.size();
    float maxIconWidth = b.w - 10;
    iconHeight = maxIconHeight;
    
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
    
    float usedHeight = iconHeight * 10;
    iconSpacing = jmax(iconSpacing, (availableHeight - usedHeight) / (float)icons.size());
    
    // setup the icons' size
    for (int i = 0; i < (int) icons.size(); ++i)
    {
        Icon* icon = icons.at(i);
        float aspectRatio = 1.0f;
        const TextureDescription& td = icon->getDefaultTexture();
        if (td.imageH != 0)
            aspectRatio = (float)td.imageW / (float)td.imageH;
        
        float width = iconHeight * aspectRatio;
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
    int N = icons.size();
    
    HUDRect buttonRect(5,
                       upButton.getBounds().y - iconHeight,
                       icons.at(0)->getBounds().w,
                       iconHeight);
    for (int i = 0; i < N; ++i)
    {
        buttonRect.w = icons.at(i)->getBounds().w;
        Icon* icon = icons.at(i);
        icon->setBounds(buttonRect);
        buttonRect.y -= iconHeight + iconSpacing;
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
    addChild(icon);
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
    }
    else if (b == &downButton)
    {
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
{
}

ListSelector::Icon::~Icon()
{
}

void ListSelector::Icon::draw()
{
    HUDView::draw();
}

void ListSelector::Icon::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
}


void ListSelector::Icon::updateBounds()
{
}
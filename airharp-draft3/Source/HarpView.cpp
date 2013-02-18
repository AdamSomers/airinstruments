#include "HarpView.h"
#include "Environment.h"

#define NUM_STRINGS 24

HarpView::HarpView(int num)
: harpNum(num)
{
    for (int i = 0; i < NUM_STRINGS; ++i)
    {
        StringView* sv = new StringView;
        sv->setup();
        sv->stringNum = i;
        sv->harpNum = harpNum;
        strings.push_back(sv);
    }
    
    for (int i = 0; i < NUM_STRINGS-1; ++i)
        HarpManager::instance().getHarp(harpNum)->AddString();
}

void HarpView::draw()
{
    stringLock.lock();
    for (StringView* sv : strings)
        sv->draw();
    stringLock.unlock();
}

void HarpView::update()
{
    stringLock.lock();
    for (StringView* sv : strings)
        sv->update();
    stringLock.unlock();
}

void HarpView::layoutStrings()
{
    stringLock.lock();
    float aspectRatio = Environment::screenW / (float)Environment::screenH;
    float stringWidth = (2.f * aspectRatio) / (float)strings.size();
    float xPos = -aspectRatio + stringWidth;
    float height = 2.f / (float)numHarps;
    float yPos = (-1.f + (height / 2.f)) + height * harpNum;
    float step = stringWidth;
    for (StringView* sv : strings)
    {
        sv->stringWidth = stringWidth;
        sv->objectFrame.SetOrigin(xPos, yPos, -12);
        sv->yScale = height / 2.f - 0.02;
        xPos += step;
        sv->updateStringBg();
    }
    stringLock.unlock();
}

void HarpView::addString()
{
    if (strings.size() >= NUM_STRINGS)
        return;
    
    Environment::openGLContext.deactivateCurrentContext();
    StringView* sv = inactiveStrings.back();
    inactiveStrings.pop_back();
    //sv->setup();
    sv->stringNum = strings.back()->stringNum + 1;
    strings.push_back(sv);
    HarpManager::instance().getHarp(harpNum)->AddString();
    layoutStrings();
    Environment::openGLContext.makeActive();
}

void HarpView::removeString()
{
    if (strings.size() <= 1)
        return;
    Environment::openGLContext.deactivateCurrentContext();
    stringLock.lock();
    StringView* sv = strings.back();
    strings.pop_back();
    inactiveStrings.push_back(sv);
    HarpManager::instance().getHarp(harpNum)->RemoveString();
    stringLock.unlock();
    layoutStrings();
    Environment::openGLContext.makeActive();
}

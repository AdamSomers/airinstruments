//
//  BeatCountView.cpp
//  AirBeats
//
//  Created by Adam Somers on 5/20/13.
//
//

#include "BeatCountView.h"

#include "Drums.h"
#include "SkinManager.h"

BeatCountView::BeatCountView()
{
    for (int i = 0; i < 32; ++i)
    {
        SharedPtr<HUDView> tickView(new HUDView);
        tickViews.push_back(tickView);
        addChild(tickView.get());
    }
    
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
}

void BeatCountView::draw()
{
    TextureDescription onTex = SkinManager::instance().getSelectedSkin().getTexture("led_on");
    TextureDescription offTex = SkinManager::instance().getSelectedSkin().getTexture("led_off");
    for (int i = 0; i < 32; ++i)
    {
        if (Drums::instance().getCurrentStep() == i)
            tickViews.at(i)->setDefaultTexture(onTex);
        else
            tickViews.at(i)->setDefaultTexture(offTex);
                
    }
    HUDView::draw();
}

void BeatCountView::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    HUDRect r = b;
    r.x = 0;
    r.w = 7;
    r.h = 7;
    r.y = b.h - r.h;
    for (int i = 0; i < 32; ++i)
    {
        tickViews.at(i)->setBounds(r);
        r.x += r.w + 3;
        if (i == 15)
        {
            r.x = 0;
            r.y -= r.h + 3;
        }
    }
}
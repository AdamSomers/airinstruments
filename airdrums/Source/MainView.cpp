//
//  MainView.cpp
//  AirBeats
//
//  Created by Adam Somers on 6/11/13.
//
//

#include "MainView.h"
#include "SkinManager.h"

MainView::MainView()
{
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
    for (int i = 0; i < 6; ++i)
    {
        SharedPtr<TextHUDButton> assign(new TextHUDButton);
        assign->setId(i);
        addChild(assign.get());
        assign->setText(StringArray("Assign"), StringArray("Assign"));
        assign->setVisible(false, 0);
        assign->setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
        assign->addListener(this);
        assignButtons.push_back(assign);
        
        SharedPtr<TextHUDButton> clear(new TextHUDButton);
        clear->setId(i);
        addChild(clear.get());
        clear->setText(StringArray("Clear"), StringArray("Clear"));
        clear->setVisible(false, 0);
        clear->setRingTexture(SkinManager::instance().getSelectedSkin().getTexture("ring"));
        clear->addListener(this);
        clearButtons.push_back(clear);
    }
}

MainView::~MainView()
{
    assignButtons.clear();
    clearButtons.clear();
}

void MainView::setBounds(const HUDRect& b)
{
    HUDView::setBounds(b);
    
    float buttonWidth = 150;
    float buttonHeight = 150;
    for (auto button : assignButtons)
        button->setBounds(HUDRect(0,0,buttonWidth,buttonHeight));
    for (auto button : clearButtons)
        button->setBounds(HUDRect(0,0,buttonWidth,buttonHeight));
}

void MainView::buttonStateChanged(HUDButton* b)
{
    for (int i = 0; i < (int)assignButtons.size(); ++i)
    {
        if (b == assignButtons.at(i).get())
            sendActionMessage("assign/" + String(b->getId()));
    }
    for (int i = 0; i < (int)clearButtons.size(); ++i)
    {
        if (b == clearButtons.at(i).get())
            sendActionMessage("clear/" + String(b->getId()));
    }
}

void MainView::setPadPos(int padIndex, GLfloat x, GLfloat y)
{
    if (padIndex < 0 || padIndex >= (int)assignButtons.size()) {
        Logger::writeToLog("Error: tried to set pad pos out of range");
        return;
    }
    
    SharedPtr<TextHUDButton> assignButton = assignButtons.at(padIndex);
    assignButton->setBounds(HUDRect(x - assignButton->getBounds().w / 2.f,
                                    y - assignButton->getBounds().h / 2.f + 20,
                                    assignButton->getBounds().w,
                                    assignButton->getBounds().h));
    
    SharedPtr<TextHUDButton> clearButton = clearButtons.at(padIndex);
    clearButton->setBounds(HUDRect(x - clearButton->getBounds().w / 2.f,
                                   y - clearButton->getBounds().h / 2.f + 20,
                                   clearButton->getBounds().w,
                                   clearButton->getBounds().h));
}

void MainView::enableAssignButtons(bool shouldBeEnabled)
{
    for (auto button : assignButtons) {
        button->setVisible(shouldBeEnabled);
        button->setEnabled(shouldBeEnabled);
    }
}

void MainView::enableClearButtons(bool shouldBeEnabled)
{
    for (auto button : clearButtons) {
        button->setVisible(shouldBeEnabled);
        button->setEnabled(shouldBeEnabled);
    }
}
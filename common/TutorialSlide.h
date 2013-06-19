//
//  TutorialSlide.h
//
//  Created by Adam Somers on 4/10/13.
//
//

#ifndef h_TutorialSlide
#define h_TutorialSlide

#include <iostream>

#include "HUD.h"
#include "TextButton.h"
#include "Types.h"

class TutorialSlide : public HUDView
                    , public HUDButton::Listener
                    , public ActionBroadcaster
{
public:
    TutorialSlide();
    ~TutorialSlide();
    
    // HUDView overrides
    void draw();
    void setBounds(const HUDRect &b);
    void loadTextures();
    void setVisible(bool shouldBeVisible, int fadeTimeMs = 500);
    
    // HUDButton::Listener override
    void buttonStateChanged(HUDButton* b);
    
    const int getSlideIndex() const { return slideIndex; }
    void setSlideIndex(int newSlideIndex);
    
    void setButtonRingTexture(TextureDescription texture);

private:
    TextHUDButton nextButton;
    TextHUDButton prevButton;
    TextHUDButton skipButton;
    
    class SlideContents : public HUDView
    {
    public:
        SlideContents();
        void init();
        void draw();
        void setBounds(const HUDRect &b);
        StringArray text;
        std::vector<SharedPtr<HUDView> > textViews;
        HUDView imageView;
        bool boundsChanged;
    };

    int slideIndex;
    std::vector<SharedPtr<SlideContents> > slides;
    
    std::vector<SharedPtr<HUDView> > offDots;
    std::vector<SharedPtr<HUDView> > onDots;
};

#endif // h_TutorialSlide

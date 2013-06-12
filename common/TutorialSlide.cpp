//
//  TutorialSlide.cpp
//
//  Created by Adam Somers on 4/10/13.
//
//

#include "TutorialSlide.h"
#include "GfxTools.h"

TutorialSlide::TutorialSlide()
: slideIndex(0)
{
    addChild(&nextButton);
    addChild(&prevButton);
    addChild(&skipButton);
    nextButton.addListener(this);
    prevButton.addListener(this);
    skipButton.addListener(this);
    nextButton.setText(StringArray("next"), StringArray("next"));
    prevButton.setText(StringArray("back"), StringArray("back"));
    StringArray skipText("Dont");
    skipText.add("Show");
    skipText.add("Again");
    skipButton.setText(skipText, skipText);
    GLfloat color[4] = { 0.f, 0.f, 0.f, 0.f };
    setDefaultColor(color);
    prevButton.setVisible(false);
}

TutorialSlide::~TutorialSlide()
{
}

// HUDView overrides
void TutorialSlide::draw()
{
    HUDView::draw();
}

void TutorialSlide::setBounds(const HUDRect &b)
{
    HUDView::setBounds(b);
    float buttonHeight = 100;
    float buttonWidth = 100;
    prevButton.setBounds(HUDRect(10, b.h / 2.f - buttonWidth / 2.f, buttonWidth, buttonHeight));
    nextButton.setBounds(HUDRect(b.w - buttonWidth - 10, b.h / 2.f - buttonWidth / 2.f, buttonWidth, buttonHeight));
    buttonHeight /= 2.f;
    buttonWidth /= 2.f;
    skipButton.setBounds(HUDRect(nextButton.getBounds().x + nextButton.getBounds().w / 2.f - buttonWidth / 2.f,
                                 nextButton.getBounds().y - buttonHeight - 30,
                                 buttonWidth,
                                 buttonHeight));
    for (SharedPtr<SlideContents> slide : slides)
        slide->setBounds(HUDRect(0,0,b.w,b.h));
}

void TutorialSlide::loadTextures()
{
    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File resourcesFile = special.getChildFile("../");
#elif JUCE_MAC
	File resourcesFile = special.getChildFile("Contents/Resources");
#endif
    File tutorialDir = resourcesFile.getChildFile("tutorial");
    
    File tutorialXml = tutorialDir.getChildFile("tutorial.xml");
    if (!tutorialXml.exists()) {
        Logger::writeToLog("Error: tutorial.xml not found!");
        return;
    }
    
    UniquePtr<XmlElement> document(XmlDocument::parse(tutorialXml));
    if (document == nullptr) {
        Logger::writeToLog("Error: Could not parse " + tutorialXml.getFileName());
        return;
    }
    
    if (!document->hasTagName("Tutorial")) {
        Logger::writeToLog("Error: Tutorial tag not found in " + tutorialXml.getFileName());
        return;
    }
    
    XmlElement* slideElement = document->getFirstChildElement();

    while (slideElement != nullptr)
    {
        SharedPtr<SlideContents> slide(new SlideContents);
        String imageFileName = slideElement->getStringAttribute("image");
        File imageFile = tutorialDir.getChildFile(imageFileName);
        if (!imageFile.exists()) {
            Logger::writeToLog("Error: image not found: " + imageFileName);
            return;
        }
        slide->imageView.setDefaultTexture(GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom(imageFile)));
        slide->imageView.setDefaultBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        XmlElement* lineElement = slideElement->getChildByName("line");
        StringArray lines;
        while (lineElement != nullptr)
        {
            String text = lineElement->getStringAttribute("text");
            lines.add(text);
            lineElement = lineElement->getNextElementWithTagName("line");
        }
        slide->text = lines;
        slide->init();
        slide->setVisible(false, 0);
        addChild(slide.get());
        slides.push_back(slide);
        slideElement = slideElement->getNextElementWithTagName("slide");
    }
    
    if (slides.size() > 0)
        slides.front()->setVisible(true, 2000);
}

void TutorialSlide::setVisible(bool shouldBeVisible, int fadeTimeMs)
{
    HUDView::setVisible(shouldBeVisible, fadeTimeMs);
    for (int i = 0; i < (int)slides.size(); ++i)
    {
        if (i != slideIndex)
            slides.at(i)->setVisible(false, 0);
    }
    if (slideIndex == 0)
        prevButton.setVisible(false, 0);

    if (slideIndex >= (int)slides.size() - 1)
        nextButton.setText(StringArray("done"), StringArray("done"));
    else
        nextButton.setText(StringArray("next"), StringArray("next"));
}

void TutorialSlide::buttonStateChanged(HUDButton* b)
{
    if (b == &nextButton)
        setSlideIndex(getSlideIndex() + 1);
    else if (b == &prevButton)
        setSlideIndex(jmax(0, getSlideIndex() - 1));
    else if (b == &skipButton) {
        setSlideIndex(slides.size());
        sendActionMessage("disableTutorial");
    }
}

void TutorialSlide::setSlideIndex(int newSlideIndex)
{
    if (newSlideIndex == (int)slides.size()) {
        setVisible(false, 1000);
        slideIndex = 0;
        sendActionMessage("tutorialDone");
    }
    else {
        slides.at(slideIndex)->setVisible(false);
        slideIndex = newSlideIndex;
        slides.at(slideIndex)->setVisible(true);
        
        if (slideIndex == (int)slides.size() - 1)
            nextButton.setText(StringArray("done"), StringArray("done"));
        else
            nextButton.setText(StringArray("next"), StringArray("next"));
        
        if (slideIndex > 0)
            prevButton.setVisible(true);
        else
            prevButton.setVisible(false);
    }
    
}

void TutorialSlide::setButtonRingTexture(TextureDescription texture)
{
    nextButton.setRingTexture(texture);
    prevButton.setRingTexture(texture);
    skipButton.setRingTexture(texture);
}

TutorialSlide::SlideContents::SlideContents()
: boundsChanged(false)
{
}

void TutorialSlide::SlideContents::init()
{
    addChild(&imageView);
    for (int i = 0; i < text.size(); ++i)
    {
        SharedPtr<HUDView> textView(new HUDView);
        textView->setMultiplyAlpha(true);
        textViews.push_back(textView);
        addChild(textView.get());
    }

    for (SharedPtr<HUDView> tv : textViews)
    {
        addChild(tv.get());
    }
}

void TutorialSlide::SlideContents::draw()
{
    jassert(text.size() == (int)textViews.size());

    if (boundsChanged && isVisible())
    {
        for (int i = 0; i < text.size(); ++i)
        {
            SharedPtr<HUDView> textView = textViews.at(i);
            Image im(Image::PixelFormat::ARGB, (int) (textView->getBounds().w*4), (int) (textView->getBounds().h*4), true);
            Graphics g (im);
            g.setColour(Colours::white);
            g.setFont(Font(Environment::instance().getDefaultFont(), (textView->getBounds().h * .75f) * 4.0f, Font::plain));
            g.drawText(text[i], 0, 0, (int) (textView->getBounds().w*4), (int) (textView->getBounds().h*4), Justification::centred, true);
            if (textView->getDefaultTexture().textureId != 0)
                glDeleteTextures(1, &textView->getDefaultTexture().textureId);
            textView->setDefaultTexture(GfxTools::loadTextureFromJuceImage(im));
        }
        boundsChanged = false;
    }

    HUDView::draw();
}

void TutorialSlide::SlideContents::setBounds(const HUDRect &b)
{
    float textY = b.h - 36;
    for (SharedPtr<HUDView> textView : textViews)
    {
        textView->setBounds(HUDRect(0, textY, b.w, 36));
        textY -= 36;
    }
    
    SharedPtr<HUDView> textView = textViews.back();
    if (imageView.getDefaultTexture().textureId != 0)
    {
        float aspectRatio = imageView.getDefaultTexture().imageH / (float)imageView.getDefaultTexture().imageW;
        float imageViewW = jmin(b.w, (float)imageView.getDefaultTexture().imageW);
        float imageViewH = imageViewW * aspectRatio;
        imageView.setBounds(HUDRect(b.w / 2.f - imageViewW / 2.f, b.h / 2.f - imageViewH / 2.f, imageViewW, imageViewH));
    }
    boundsChanged = true;
}

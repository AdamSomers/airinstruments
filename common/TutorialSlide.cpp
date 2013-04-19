//
//  TutorialSlide.cpp
//
//  Created by Adam Somers on 4/10/13.
//
//

#include "TutorialSlide.h"
#include "GfxTools.h"

TutorialSlide::TutorialSlide()
: fade(0.f)
, fadeTarget(0.f)
, slideIndex(0)
, nextSlide(0)
, done(false)
{
    
}

TutorialSlide::~TutorialSlide()
{
    
}

// HUDView overrides
void TutorialSlide::draw()
{
    GLint blendSrc;
    glGetIntegerv(GL_BLEND_SRC, &blendSrc);
    GLint blendDst;
    glGetIntegerv(GL_BLEND_DST, &blendDst);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat texColor[4] = { 1.f, 1.f, 1.f, fade };
    glBindTexture(GL_TEXTURE_2D, textures[slideIndex]);
    Environment::instance().shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, Environment::instance().transformPipeline.GetModelViewMatrix(), texColor, 0);
    defaultBatch.Draw();

    glBlendFunc(blendSrc, blendDst);
    
    if (fade > fadeTarget)
    {
        fade -= 0.05f;
        if (fade < 0.f) fade = 0.f;
        if (fade == 0.f && slideIndex != nextSlide && !done)
        {
            slideIndex = nextSlide;
            fadeTarget = 1.f;
        }
    }
    else if (fade < fadeTarget)
    {
        fade += 0.05f;
        if (fade > 1.f) fade = 1.f;
    }
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
    DirectoryIterator imagesIter(tutorialDir, true, "*.png", File::findFiles);
    while (imagesIter.next())
    {
        File imageFile = imagesIter.getFile();
        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (imageFile));
        textures.add(textureId);
    }
}

void TutorialSlide::next()
{
    fadeTarget = 0.f;
    if (slideIndex != textures.size() - 1)
        nextSlide = slideIndex + 1;
    else
        done = true;
}

void TutorialSlide::back()
{
    if (slideIndex != 0) {
        nextSlide = slideIndex - 1;
        fadeTarget = 0.f;
    }
}

void TutorialSlide::end()
{
    fadeTarget = 0.f;
    done = true;
}

void TutorialSlide::begin()
{
    fade = 0.f;
    fadeTarget = 1.f;
    slideIndex = 0;
    done = false;
}

bool TutorialSlide::isDone()
{
    return done;
}

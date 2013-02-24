#include "SkinManager.h"
#include "GfxTools.h"

SkinManager::SkinManager()
{
}

SkinManager::~SkinManager()
{
}

void SkinManager::loadResources()
{
    File appDataFile = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents").getChildFile("Resources");

    bool more = true;
    int count = 0;
    while (more == true) {
        String number(count);
        File backgroundFile = appDataFile.getChildFile("background" + number + ".png");
        File buttonOnFile = appDataFile.getChildFile("button_On" + number + ".png");
        File buttonOffFile = appDataFile.getChildFile("button_Off" + number + ".png");
        File bezelTopFile = appDataFile.getChildFile("bezel_Top" + number + ".png");
        File bezelBottomFile = appDataFile.getChildFile("bezel_Bottom" + number + ".png");
        File stringFile = appDataFile.getChildFile("string" + number + ".png");
        File stringBackgroundFile = appDataFile.getChildFile("stringBg" + number + ".png");
        
        if (backgroundFile.exists() &&
            buttonOnFile.exists() &&
            buttonOffFile.exists() &&
            bezelTopFile.exists() &&
            bezelBottomFile.exists() &&
            stringFile.exists() &&
            stringBackgroundFile.exists())
        {
            Skin s;
            glGenTextures(1, &s.background);
            glBindTexture(GL_TEXTURE_2D, s.background);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (backgroundFile));
            glGenTextures(1, &s.buttonOn);
            glBindTexture(GL_TEXTURE_2D, s.buttonOn);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (buttonOnFile));
            glGenTextures(1, &s.buttonOff);
            glBindTexture(GL_TEXTURE_2D, s.buttonOff);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (buttonOffFile));
            glGenTextures(1, &s.bezelTop);
            glBindTexture(GL_TEXTURE_2D, s.bezelTop);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (bezelTopFile));
            glGenTextures(1, &s.bezelBottom);
            glBindTexture(GL_TEXTURE_2D, s.bezelBottom);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (bezelBottomFile));
            glGenTextures(1, &s.string);
            glBindTexture(GL_TEXTURE_2D, s.string);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (stringFile));
            glGenTextures(1, &s.stringBackground);
            glBindTexture(GL_TEXTURE_2D, s.stringBackground);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (stringBackgroundFile));
            skins.push_back(s);
            count++;
        }
        else
        {
            more = false;
        }
    }
}

const Skin& SkinManager::getSkin()
{
    if (skins.size() == 0)
        loadResources();
    
    jassert(skins.size() > 0);
    return skins.at(currentSkinIndex);
}

void SkinManager::setSkinIndex(unsigned int skin)
{
    if (skin >= skins.size())
        skin = skins.size() - 1;
    currentSkinIndex = skin;
}

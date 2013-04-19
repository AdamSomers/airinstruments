#include "GfxTools.h"
#include "SkinManager.h"

Skin::Skin(String skinName)
: name(skinName)
{
}

const String& Skin::getName() const
{
    return name;
}

const GLuint Skin::getTexture(String name) const
{
    GLuint tex = 0;
    try {
        tex = textures.at(name);
    }
    catch (const std::out_of_range& /*oor*/) {
        Logger::outputDebugString("No texture for " + name);
    }
    return tex;
}

void Skin::addTexture(String name, GLuint texture)
{
    auto iter = textures.find(name);
    if (iter != textures.end())
        Logger::outputDebugString("Warning: overwriting texture " + String(iter->second) + " for " + name + " with " + String(texture));
    textures.insert(std::make_pair(name, texture));
}

SkinManager::SkinManager()
{
    selectedSkinName = "Labour";
}

SkinManager::~SkinManager()
{
}

void SkinManager::loadResources()
{    
    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File resourcesFile = special.getChildFile("../");
#elif JUCE_MAC
	File resourcesFile = special.getChildFile("Contents/Resources");
#endif
    
    //printf("SkinManager::loadResources - skins:\n");
    DirectoryIterator iter(resourcesFile.getChildFile("skins"), false, "*", File::findDirectories);
    while(iter.next())
    {
        File skinDir = iter.getFile();
        String skinName = skinDir.getFileName();
        Skin s(skinName);
        //Logger::outputDebugString("\t" + skinName);
        DirectoryIterator skinImagesIter(skinDir, true, "*.png", File::findFiles);
        while (skinImagesIter.next())
        {
            File imageFile = skinImagesIter.getFile();
            String imageName = imageFile.getFileNameWithoutExtension();
            //Logger::outputDebugString("\t\t" + imageName);
            GLuint textureId = 0;
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);
            GfxTools::loadTextureFromJuceImage(ImageFileFormat::loadFrom (imageFile));
            s.addTexture(imageName, textureId);
        }
        skins.insert(std::make_pair(skinName, s));
    }
}

const Skin& SkinManager::getSkin(String name) const
{    
    jassert(skins.size() > 0);
    try {
        return skins.at(name);
    }
    catch (const std::out_of_range& /*oor*/) {
        Logger::outputDebugString("No skin named " + name);
        return skins.begin()->second;
    }
}

const Skin& SkinManager::getSelectedSkin() const
{
    return getSkin(selectedSkinName);
}

void SkinManager::setSelectedSkin(const String& name)
{
    selectedSkinName = name;
}

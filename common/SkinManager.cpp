#include "GfxTools.h"
#include "SkinManager.h"
#include "Types.h"

Skin::Skin(String skinName)
: name(skinName)
{
}

const String& Skin::getName() const
{
    return name;
}

const TextureDescription Skin::getTexture(String name) const
{
    TextureDescription tex;
    try {
        tex = textures.at(name);
    }
    catch (const std::out_of_range& /*oor*/) {
        Logger::writeToLog("No texture for " + name);
    }
    return tex;
}

void Skin::addTexture(String name, TextureDescription texture)
{
    auto iter = textures.find(name);
    if (iter != textures.end())
        Logger::writeToLog("Warning: overwriting texture " + String(iter->second.textureId) + " for " + name + " with " + String(texture.textureId));
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
    Logger::writeToLog("loadResources begin");
    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File resourcesFile = special.getChildFile("../");
#elif JUCE_MAC
	File resourcesFile = special.getChildFile("Contents/Resources");
#endif
    
    Logger::writeToLog("resources file: " + resourcesFile.getFullPathName());
    File skinsFile = resourcesFile.getChildFile("skins");
    Logger::writeToLog("skins file: " + skinsFile.getFullPathName());
    Logger::writeToLog("skins file existence: " + String(skinsFile.exists()));
    DirectoryIterator iter(skinsFile, false, "*", File::findDirectories);
    while(iter.next())
    {
        File skinDir = iter.getFile();
        String skinName = skinDir.getFileName();
        Skin s(skinName);
        Logger::writeToLog("skin: " + skinName);
        DirectoryIterator skinImagesIter(skinDir, true, "*.png", File::findFiles);
        while (skinImagesIter.next())
        {
            File imageFile = skinImagesIter.getFile();
            Logger::writeToLog("\tfile: " + imageFile.getFullPathName());
            Image image = ImageFileFormat::loadFrom (imageFile);
            Logger::writeToLog("\timage validity: " + String(image.isValid()));
            TextureDescription textureDesc = GfxTools::loadTextureFromJuceImage(image);
            String imageName = imageFile.getFileNameWithoutExtension();
            File atlasXml = imageFile.getParentDirectory().getChildFile(imageName + ".xml");
            if (atlasXml.exists())
            {
                Logger::writeToLog(imageFile.getFileName() + " is an atlas!");
                
                Array<TextureDescription> textures = GfxTools::loadTextureAtlas(atlasXml);
                for (int i = 0; i < textures.size(); ++i)
                {
                    TextureDescription td = textures[i];
                    s.addTexture(td.name, td);
                }
            }
            else
            {
                s.addTexture(imageName, textureDesc);
            }
        }
        skins.insert(std::make_pair(skinName, s));
    }
    Logger::writeToLog("loadResources end");
}

const Skin& SkinManager::getSkin(String name) const
{    
    jassert(skins.size() > 0);
    try {
        return skins.at(name);
    }
    catch (const std::out_of_range& /*oor*/) {
        Logger::writeToLog("No skin named " + name);
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

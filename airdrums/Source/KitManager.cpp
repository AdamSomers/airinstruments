/*
  ==============================================================================

    KitManager.cpp
    Created: 25 Mar 2013 1:39:31pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "KitManager.h"


KitManager::KitManager()
{
	File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File folder = special.getChildFile("../kits");
#elif JUCE_MAC
	File folder = special.getChildFile("Contents/Resources/kits");
#endif
	mFactoryPath = folder.getFullPathName();
}


KitManager::~KitManager()
{
}

KitManager::Status KitManager::BuildKitList()
{
    StringArray paths(mFactoryPath);
    return BuildKitList(paths);
}

KitManager::Status KitManager::BuildKitList(StringArray paths, bool clear /*= true*/)
{
    Logger::writeToLog("KitManager::BuildKitList()");

	if (clear)
	{
		mItems.clear();
	}

	return ItemManager<KitManager, DrumKit>::BuildItemList("*.xml", "kit", paths, false);
}

void KitManager::LoadTextures()
{
    Logger::writeToLog("KitManager::LoadTextures()");
	int count = GetItemCount();
    for (int i = 0; i < count; ++i)
    {
		SharedPtr<DrumKit> kit = GetItem(i);
        kit->LoadTextures();
    }
    
    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File folder = special.getChildFile("..");
#elif JUCE_MAC
	File folder = special.getChildFile("Contents/Resources");
#endif

    File directory(folder);
    
	if (!directory.isDirectory()) {
		Logger::writeToLog("Error: " + folder.getFileName() + " not a directory");
        return;
    }
    
    File atlasXml = directory.getChildFile("kitsAtlas.xml");
    Array<TextureDescription> textureArray = GfxTools::loadTextureAtlas(atlasXml);
    
    for (int i = 0; i < textureArray.size(); ++i)
    {
        TextureDescription td = textureArray[i];
        for (int i = 0; i < count; ++i)
        {
            SharedPtr<DrumKit> kit = GetItem(i);
            if (kit->GetName() == td.name && kit->GetImage().isNull()) {
                kit->SetTexture(td);
                break;
            }
        }
    }
}

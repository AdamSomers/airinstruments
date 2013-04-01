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
	mDefaultPath = folder.getFullPathName();
}


KitManager::~KitManager()
{
}


KitManager::Status KitManager::BuildKitList(String path /* = ""*/, bool clear /*= true*/)
{
	return ItemManager<KitManager, DrumKit>::BuildItemList("*.xml", "kit", path, clear);
}

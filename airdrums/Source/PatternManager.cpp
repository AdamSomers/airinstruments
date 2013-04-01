/*
  ==============================================================================

    PatternManager.cpp
    Created: 29 Mar 2013 2:41:13pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "PatternManager.h"


PatternManager::PatternManager()
{
	File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File folder = special.getChildFile("../patterns");
#elif JUCE_MAC
	File folder = special.getChildFile("Contents/Resources/patterns");
#endif
	mDefaultPath = folder.getFullPathName();
}


PatternManager::~PatternManager()
{
}


PatternManager::Status PatternManager::BuildPatternList(String path /* = ""*/, bool clear /*= true*/)
{
	return ItemManager<PatternManager, DrumPattern>::BuildItemList("*.xml", "pattern", path, clear);
}

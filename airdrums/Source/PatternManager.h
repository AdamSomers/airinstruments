/*
  ==============================================================================

    PatternManager.h
    Created: 29 Mar 2013 2:41:28pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __PATTERNMANAGER_H_9CB8DC32__
#define __PATTERNMANAGER_H_9CB8DC32__

#include "../JuceLibraryCode/JuceHeader.h"

#include "ItemManager.h"
#include "DrumPattern.h"


class PatternManager : public ItemManager<PatternManager, DrumPattern>
{
public:
	Status	BuildPatternList(String path = "", bool clear = true);

private:
	friend class ItemManager<PatternManager, DrumPattern>;
	PatternManager();
	~PatternManager();
};

#endif  // __PATTERNMANAGER_H_9CB8DC32__

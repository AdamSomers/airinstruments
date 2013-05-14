/*
  ==============================================================================

    PatternManager.h
    Created: 29 Mar 2013 2:41:28pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __PATTERNMANAGER_H_9CB8DC32__
#define __PATTERNMANAGER_H_9CB8DC32__

#include "Main.h"
#include "DrumPattern.h"
#include "ItemManager.h"

#include "../JuceLibraryCode/JuceHeader.h"


class PatternManager : public ItemManager<PatternManager, DrumPattern>
{
public:
	Status	BuildPatternList(String path = "", bool clear = true);
	Status	SavePattern(AirHarpApplication::MainWindow* mainWindow);
	Status	SavePatternAs(AirHarpApplication::MainWindow* mainWindow);
	Status	LoadPattern(AirHarpApplication::MainWindow* mainWindow);
	Status	UsePatternTempo(bool usePatternTempo);
	Status	CreateNewPattern(void);		// Creates a new, modifiable, unnamed, empty pattern and sets it as the current pattern in the Drums object

private:
	friend class ItemManager<PatternManager, DrumPattern>;
	PatternManager();
	~PatternManager();
};

#endif  // __PATTERNMANAGER_H_9CB8DC32__

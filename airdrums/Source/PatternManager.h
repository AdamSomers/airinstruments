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
	Status	BuildPatternList(StringArray paths, bool clear = true);
    Status	BuildPatternList();
	Status	SavePattern(void);
	Status	SavePatternAs(void);
	Status	LoadPattern(void);
	Status	UsePatternTempo(bool usePatternTempo);
	Status	CreateNewPattern(void);			// Creates a new, modifiable, default named, empty pattern and sets it as the current pattern in the Drums object
	Status	SaveDirtyPatternPrompt(void);

private:
	void	UpdatePrefsLastPattern(SharedPtr<DrumPattern> pattern);
	void	UpdatePatternWheel(void);

	friend class ItemManager<PatternManager, DrumPattern>;
	PatternManager();
	~PatternManager();
};

#endif  // __PATTERNMANAGER_H_9CB8DC32__

/*
  ==============================================================================

    KitManager.h
    Created: 25 Mar 2013 1:39:31pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __KITMANAGER_H_C9AB6D2C__
#define __KITMANAGER_H_C9AB6D2C__

#include "../JuceLibraryCode/JuceHeader.h"

#include "ItemManager.h"
#include "DrumKit.h"


class KitManager : public ItemManager<KitManager, DrumKit>
{
public:
    // call me from OpenGL thread only!
    void LoadTextures();
	Status	BuildKitList(String path = "", bool clear = true);

private:
	friend class ItemManager<KitManager, DrumKit>;
	KitManager();
	~KitManager();
};

#endif  // __KITMANAGER_H_C9AB6D2C__

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

#include <vector>

#include "Types.h"
#include "DrumKit.h"


class KitManager
{
public:
	enum Status
	{
		kNoError = 0,
		kPathNotFoundError = 1,
		kXmlParseError = 2,
		kKitLoadError = 3,
		kNoKitsError = 4
	};

	~KitManager();

	static KitManager&	GetInstance(void);
	static void			Destruct(void);

	int		GetKitCount(void);

	SharedPtr<DrumKit>	GetKit(int index);

	//void	SetPathToKitFolder(String path);
	Status	BuildKitList(String path = "");
    
    // call me from OpenGL thread only!
    void LoadTextures();

private:
	KitManager();

	static KitManager*	mSelf;

	typedef	SharedPtr<DrumKit>		Item;
	typedef	std::vector<Item>		Container;
	typedef	Container::iterator		Iterator;

	Container	mKits;
	String		mDefaultPath;
};

#endif  // __KITMANAGER_H_C9AB6D2C__

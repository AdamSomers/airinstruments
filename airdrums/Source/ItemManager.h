/*
  ==============================================================================

    ItemManager.h
    Created: 29 Mar 2013 6:26:52pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __ITEMMANAGER_H_7CE92AF9__
#define __ITEMMANAGER_H_7CE92AF9__

#include "../JuceLibraryCode/JuceHeader.h"

#include <vector>

#include "Types.h"


// This is the templated base class from which KitManager and PatternManager derive.
// Template parameter Managed must derive from class DrumItem
template <typename Manager, typename Managed>
class ItemManager
{
public:
	enum Status
	{
		kNoError = 0,
		kPathNotFoundError = 1,
		kXmlParseError = 2,
		kItemLoadError = 3,
		kNoItemsError = 4
	};

	static Manager&	GetInstance(void);
	static void		Destruct(void);

	int		GetItemCount(void);

	SharedPtr<Managed>	GetItem(int index);
	SharedPtr<Managed>	GetItem(Uuid& uuid);

	//void	SetPathToItemFolder(String path);

	String& GetDefaultPath(void);

protected:
	Status	BuildItemList(String fileExtension, String xmlTag, String path = "", bool clear = true);

private:
	friend typename Manager;
	ItemManager();
	~ItemManager();

	static Manager*	mSelf;

	typedef	SharedPtr<typename Managed>		Item;
	typedef	std::vector<typename Item>		Container;
	typedef	typename Container::iterator	Iterator;

	Container	mItems;
	String		mDefaultPath;
};

// I dislike code in header files, so I put all definition in a cpp file and include it here.
// This code needs to be available in the header at compile time since the class is a template.
// This cpp file should not be compiled separately in the build process, but only included here.
#include "ItemManager.cpp"

#endif  // __ITEMMANAGER_H_7CE92AF9__

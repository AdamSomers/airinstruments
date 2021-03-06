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
		kNoItemsError = 4,
		kCancelled = 5,
		kSaveError = 6
	};

	static Manager&	GetInstance(void);
	static void		Destruct(void);

	int		GetItemCount(void);

	SharedPtr<Managed>	GetItem(int index);
	SharedPtr<Managed>	GetItem(Uuid& uuid);
    int GetIndexOfItem(SharedPtr<Managed> item);
	void AddItem(SharedPtr<Managed> item);

	//void	SetPathToItemFolder(String path);

	String& GetFactoryPath(void);
    String& GetUserPath(void);

protected:
	Status	BuildItemList(String fileExtension, String xmlTag, StringArray paths, bool clear = true);

private:
	friend Manager;
	ItemManager();
	~ItemManager();

	static Manager*	mSelf;

	typedef	SharedPtr<Managed>		Item;
	typedef	std::vector<Item>		Container;
	typedef	typename Container::iterator	Iterator;

	Container	mItems;
	String		mFactoryPath;
    String      mUserPath;
};

// I dislike code in header files, so I put all definition in a cpp file and include it here.
// This code needs to be available in the header at compile time since the class is a template.
// This cpp file should not be compiled separately in the build process, but only included here.
#include "ItemManager.cpp"

#endif  // __ITEMMANAGER_H_7CE92AF9__

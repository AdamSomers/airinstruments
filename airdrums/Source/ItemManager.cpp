/*
  ==============================================================================

    ItemManager.cpp
    Created: 29 Mar 2013 6:26:37pm
    Author:  Bob Nagy

  ==============================================================================
*/


template <typename Manager, typename Managed>
Manager* ItemManager<Manager, Managed>::mSelf = nullptr;


template <typename Manager, typename Managed>
ItemManager<Manager, Managed>::ItemManager()
{
}


template <typename Manager, typename Managed>
ItemManager<Manager, Managed>::~ItemManager()
{
}


template <typename Manager, typename Managed>
Manager& ItemManager<Manager, Managed>::GetInstance(void)
{
	if (mSelf == nullptr)
		mSelf = new Manager;
	return *mSelf;
}


template <typename Manager, typename Managed>
void ItemManager<Manager, Managed>::Destruct(void)
{
	if (mSelf != nullptr)
	{
		delete mSelf;
		mSelf = nullptr;
	}
}


template <typename Manager, typename Managed>
int ItemManager<Manager, Managed>::GetItemCount(void)
{
	return mItems.size();
}


template <typename Manager, typename Managed>
SharedPtr<Managed> ItemManager<Manager, Managed>::GetItem(int index)
{
	jassert(index < (int) mItems.size());
	jassert(index >= 0);

	return mItems.at(index);
}


template <typename Manager, typename Managed>
SharedPtr<Managed> ItemManager<Manager, Managed>::GetItem(Uuid& uuid)
{
	// Linear search, should be Ok since we don't expect large numbers of items
	for (int i = 0; i < (int) mItems.size(); ++i)
	{
		SharedPtr<Managed> item = mItems.at(i);
		if (item->GetUuid() == uuid)
			return item;
	}

	return SharedPtr<Managed>();
}


template <typename Manager, typename Managed>
int ItemManager<Manager, Managed>::GetIndexOfItem(SharedPtr<Managed> item)
{
    for (int i = 0; i < (int) mItems.size(); ++i)
	{
		if (mItems.at(i) == item)
			return i;
	}
    return -1;
}


template <typename Manager, typename Managed>
void ItemManager<Manager, Managed>::AddItem(SharedPtr<Managed> item)
{
	mItems.push_back(item);
}


template <typename Manager, typename Managed>
String& ItemManager<Manager, Managed>::GetFactoryPath(void)
{
	return mFactoryPath;
}

template <typename Manager, typename Managed>
String& ItemManager<Manager, Managed>::GetUserPath(void)
{
	return mUserPath;
}

template <typename Manager, typename Managed>
typename ItemManager<Manager, Managed>::Status ItemManager<Manager, Managed>::BuildItemList(String fileExtension, String xmlTag, StringArray paths, bool clear /*= true*/)
{
	if (paths.size() == 0)
		paths.add(mFactoryPath);
    
    if (clear)
        mItems.clear();

    for (int i = 0; i < paths.size(); ++i)
    {
        String path = paths[i];
        File directory(path);
        Logger::writeToLog("ItemManager::BuildItemList looking in " + path);


        if (!directory.isDirectory()) {
            Logger::writeToLog("ERROR: ItemManager::BuildItemList could not find directory " + path);
        }

        DirectoryIterator it(directory, true, fileExtension);
        while (it.next())
        {
            File file(it.getFile());
            UniquePtr<XmlElement> document(XmlDocument::parse(file));
            if (document == nullptr) {
                Logger::writeToLog("ERROR: ItemManager::BuildItemList failed to parse " + file.getFileName());
                return kXmlParseError;
            }
            if (!document->hasTagName(xmlTag)) {
                Logger::writeToLog("ERROR: ItemManager::BuildItemList did not find tag " + xmlTag + " in " + file.getFileName());
                return kXmlParseError;
            }

            SharedPtr<Managed> item(new Managed);
            File folder = file.getParentDirectory();
            typename Managed::Status status = item->LoadFromXml(document.get(), folder);
            if (status != Managed::kNoError) {
                Logger::writeToLog("ERROR: ItemManager::BuildItemList failed to load " + file.getFileName());
                return kItemLoadError;
            }
            item->SetFile(file);

            mItems.push_back(item);
        }
    }

	if (GetItemCount() == 0)
		return kNoItemsError;
	return kNoError;
}

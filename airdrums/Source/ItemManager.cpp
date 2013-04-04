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
String& ItemManager<Manager, Managed>::GetDefaultPath(void)
{
	return mDefaultPath;
}


template <typename Manager, typename Managed>
typename ItemManager<Manager, Managed>::Status ItemManager<Manager, Managed>::BuildItemList(String fileExtension, String xmlTag, String path /* = ""*/, bool clear /*= true*/)
{
	if (path == "")
		path = mDefaultPath;

	File directory(path);

	if (!directory.isDirectory())
		return kPathNotFoundError;

	if (clear)
		mItems.clear();

	DirectoryIterator it(directory, true, fileExtension);
	while (it.next())
	{
		File file(it.getFile());
		UniquePtr<XmlElement> document(XmlDocument::parse(file));
		if (document == nullptr)
			return kXmlParseError;
		if (!document->hasTagName(xmlTag))
			return kXmlParseError;

		SharedPtr<Managed> item(new Managed);
		File folder = file.getParentDirectory();
		Managed::Status status = item->LoadFromXml(document.get(), folder);
		if (status != Managed::kNoError)
			return kItemLoadError;

		mItems.push_back(item);
	}

	if (GetItemCount() == 0)
		return kNoItemsError;
	return kNoError;
}

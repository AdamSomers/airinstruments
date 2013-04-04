/*
  ==============================================================================

    KitManager.cpp
    Created: 25 Mar 2013 1:39:31pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "KitManager.h"

KitManager*	KitManager::mSelf = nullptr;


KitManager::KitManager()
{
	File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File folder = special.getChildFile("../kits");
#elif JUCE_MAC
	File folder = special.getChildFile("Contents/Resources/kits");
#endif
	mDefaultPath = folder.getFullPathName();
}


KitManager::~KitManager()
{
}


KitManager& KitManager::GetInstance(void)
{
	if (mSelf == nullptr)
		mSelf = new KitManager;
	return *mSelf;
}


void KitManager::Destruct(void)
{
	if (mSelf != nullptr)
	{
		delete mSelf;
		mSelf = nullptr;
	}
}


int KitManager::GetKitCount(void)
{
	return mKits.size();
}


SharedPtr<DrumKit> KitManager::GetKit(int index)
{
	jassert(index < (int) mKits.size());
	jassert(index >= 0);

	return mKits.at(index);
}


KitManager::Status KitManager::BuildKitList(String path /* = ""*/)
{
	if (path == "")
		path = mDefaultPath;

	File directory(path);

	if (!directory.isDirectory())
		return kPathNotFoundError;

	DirectoryIterator it(directory, true, "*.xml");
	while (it.next())
	{
		File file(it.getFile());
		UniquePtr<XmlElement> document(XmlDocument::parse(file));
		if (document == nullptr)
			return kXmlParseError;
		if (!document->hasTagName("kit"))
			return kXmlParseError;

		SharedPtr<DrumKit> kit(new DrumKit);
		File folder = file.getParentDirectory();
		DrumKit::Status status = kit->LoadFromXml(document.get(), folder);
		if (status != DrumKit::kNoError)
			return kKitLoadError;

		mKits.push_back(kit);
	}

	if (GetKitCount() == 0)
		return kNoKitsError;
	return kNoError;
}

void KitManager::LoadTextures()
{
    for (Item kit : mKits)
    {
        kit->LoadTextures();
    }
}

/*
  ==============================================================================

    KitManager.cpp
    Created: 25 Mar 2013 1:39:31pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "KitManager.h"


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


KitManager::Status KitManager::BuildKitList(String path /* = ""*/, bool clear /*= true*/)
{
	if (clear)
	{
		mItems.clear();
		//Status status = AddBuiltInKit();
		//if (status != kNoError)
		//	return status;
	}
	return ItemManager<KitManager, DrumKit>::BuildItemList("*.xml", "kit", path, false);
}


KitManager::Status KitManager::AddBuiltInKit(void)
{
	std::vector<DrumKit::MemorySampleInfo> samples;
	DrumKit::MemorySampleInfo info;

	info.data = BinaryData::TMD_CHIL_BKICK_aif;
	info.size = BinaryData::TMD_CHIL_BKICK_aifSize;
	info.note = 15;
	info.category = "BassDrum";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_CHH_aif;
	info.size = BinaryData::TMD_CHIL_CHH_aifSize;
	info.note = 4;
	info.category = "ClosedHiHat";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_CLAP_aif;
	info.size = BinaryData::TMD_CHIL_CLAP_aifSize;
	info.note = 2;
	info.category = "Clap";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_CLV_aif;
	info.size = BinaryData::TMD_CHIL_CLV_aifSize;
	info.note = 3;
	info.category = "HiPerc";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_CYM_aif;
	info.size = BinaryData::TMD_CHIL_CYM_aifSize;
	info.note = 10;
	info.category = "Crash";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_HTOM_aif;
	info.size = BinaryData::TMD_CHIL_HTOM_aifSize;
	info.note = 9;
	info.category = "HiTom";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_KICK_aif;
	info.size = BinaryData::TMD_CHIL_KICK_aifSize;
	info.note = 0;
	info.category = "BassDrum";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_LTOM_aif;
	info.size = BinaryData::TMD_CHIL_LTOM_aifSize;
	info.note = 7;
	info.category = "LowTom";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_MTOM_aif;
	info.size = BinaryData::TMD_CHIL_MTOM_aifSize;
	info.note = 8;
	info.category = "MidTom";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_OHH_aif;
	info.size = BinaryData::TMD_CHIL_OHH_aifSize;
	info.note = 6;
	info.category = "OpenHiHat";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_PAD_aif;
	info.size = BinaryData::TMD_CHIL_PAD_aifSize;
	info.note = 12;
	info.category = "Pad";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_PHH_aif;
	info.size = BinaryData::TMD_CHIL_PHH_aifSize;
	info.note = 5;
	info.category = "PedalHiHat";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_RIDE_aif;
	info.size = BinaryData::TMD_CHIL_RIDE_aifSize;
	info.note = 11;
	info.category = "Ride";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_SFX_aif;
	info.size = BinaryData::TMD_CHIL_SFX_aifSize;
	info.note = 13;
	info.category = "FX";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_SN_1_aif;
	info.size = BinaryData::TMD_CHIL_SN_1_aifSize;
	info.note = 1;
	info.category = "Snare";
	samples.push_back(info);

	info.data = BinaryData::TMD_CHIL_VERB_aif;
	info.size = BinaryData::TMD_CHIL_VERB_aifSize;
	info.note = 14;
	info.category = "FX";
	samples.push_back(info);

	Uuid uuid("0e5810c4c2a824acaac71aae0c889177");

	SharedPtr<DrumKit> kit(new DrumKit);
	DrumKit::Status status = kit->CreateFromMemory(samples, "BuiltIn", uuid);
	if (status != DrumKit::kNoError)
		return kItemLoadError;

	mItems.push_back(kit);

	return kNoError;
}


void KitManager::LoadTextures()
{
	int count = GetItemCount();
    for (int i = 0; i < count; ++i)
    {
		SharedPtr<DrumKit> kit = GetItem(i);
        kit->LoadTextures();
    }
    
    File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File folder = special.getChildFile("..");
#elif JUCE_MAC
	File folder = special.getChildFile("Contents/Resources");
#endif

    File directory(folder);
    
	if (!directory.isDirectory()) {
		Logger::outputDebugString("Error: " + folder.getFileName() + " not a directory");
        return;
    }
    
    File atlasXml = directory.getChildFile("kitsAtlas.xml");
    Array<TextureDescription> textureArray = GfxTools::loadTextureAtlas(atlasXml);
    
    for (int i = 0; i < textureArray.size(); ++i)
    {
        TextureDescription td = textureArray[i];
        for (int i = 0; i < count; ++i)
        {
            SharedPtr<DrumKit> kit = GetItem(i);
            if (kit->GetName() == td.name && kit->GetImage().isNull()) {
                kit->SetTexture(td);
                break;
            }
        }
    }
}

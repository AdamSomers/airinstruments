/*
  ==============================================================================

    PatternManager.cpp
    Created: 29 Mar 2013 2:41:13pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "PatternManager.h"
#include "PatternSaveDialog.h"
#include "PatternLoadDialog.h"


PatternManager::PatternManager()
{
	File special = File::getSpecialLocation(File::currentApplicationFile);
#if JUCE_WINDOWS
	File folder = special.getChildFile("../patterns");
#elif JUCE_MAC
	File folder = special.getChildFile("Contents/Resources/patterns");
#endif
	mDefaultPath = folder.getFullPathName();
}


PatternManager::~PatternManager()
{
}


PatternManager::Status PatternManager::BuildPatternList(String path /* = ""*/, bool clear /*= true*/)
{
	return ItemManager<PatternManager, DrumPattern>::BuildItemList("*.xml", "pattern", path, clear);
}


PatternManager::Status PatternManager::SavePattern(AirHarpApplication::MainWindow* mainWindow)
{
	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern = drums.getPattern();
	jassert(pattern.get() != nullptr);
	if (!pattern->GetDirty())
		return kCancelled;
	if (!pattern->GetHasValidName() || !pattern->GetModifiable())
		return SavePatternAs(mainWindow);

	String fileName = pattern->GetName();
	fileName = File::createLegalFileName(fileName);
	// For now, use the default path
	File directory = GetDefaultPath();

	DrumItem::Status saveStatus = pattern->SaveToXml(fileName, directory);
	if (saveStatus != DrumItem::Status::kNoError)
		return kSaveError;

	return kNoError;
}


PatternManager::Status PatternManager::SavePatternAs(AirHarpApplication::MainWindow* mainWindow)
{
	UniquePtr<PatternSaveDialog> dlg(new PatternSaveDialog(mainWindow));
	int status = dlg->runModalLoop();
	if (status == 0)
		return kCancelled;
	String fileName = dlg->GetFileName();
	fileName = File::createLegalFileName(fileName);
	// For now, use the default path
	File directory = GetDefaultPath();

	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern = drums.getPattern();
	jassert(pattern.get() != nullptr);
    pattern->SetUuid(Uuid::Uuid());
	DrumItem::Status saveStatus = pattern->SaveToXml(fileName, directory);
	if (saveStatus != DrumItem::Status::kNoError)
		return kSaveError;
            
	Status buildStatus = BuildPatternList();	// Refresh list to find new content, etc.
	if (buildStatus != kNoError)
		return buildStatus;

    String name = pattern->GetName();
    Uuid uuid = pattern->GetUuid();
    String uuidString = uuid.toString();
    AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	PropertiesFile* propsFile = props.getUserSettings();
	propsFile->setValue("patternName", name);
    propsFile->setValue("patternUuid", uuidString);
            
    AirHarpApplication::PatternAddedMessage* m = new AirHarpApplication::PatternAddedMessage;
    ((MainContentComponent*)mainWindow->getContentComponent())->postMessage(m);

	return kNoError;
}


PatternManager::Status PatternManager::LoadPattern(AirHarpApplication::MainWindow* mainWindow)
{
	Status buildStatus = BuildPatternList();	// Refresh list to find new content, etc.
	if (buildStatus != kNoError)
		return buildStatus;
	UniquePtr<PatternLoadDialog> dlg(new PatternLoadDialog(mainWindow));
	int status = dlg->runModalLoop();
	if (status == 0)
		return kCancelled;
	int index = dlg->GetSelection();
	if (index < 0)
		return kCancelled;
	SharedPtr<DrumPattern> pattern = GetItem(index);
	Drums& drums = Drums::instance();
	drums.setPattern(pattern);

	return kNoError;
}


PatternManager::Status PatternManager::UsePatternTempo(bool usePatternTempo)
{
	Drums& drums = Drums::instance();
	if (usePatternTempo)
		drums.setTempoSource(Drums::kPatternTempo);
	else
		drums.setTempoSource(Drums::kGlobalTempo);

	return kNoError;
}


PatternManager::Status PatternManager::CreateNewPattern(void)
{
	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern(new DrumPattern);
	pattern->SetModifiable(true);
	drums.setPattern(pattern);

	return kNoError;
}


PatternManager::Status PatternManager::MakePatternModifiable(void)
{
	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern = drums.getPattern();
	jassert(pattern.get() != nullptr);
	if (pattern->GetModifiable())
		return kNoError;

	SharedPtr<DrumPattern> newPattern(new DrumPattern(*pattern.get()));
	drums.setPattern(newPattern);

	return kNoError;
}

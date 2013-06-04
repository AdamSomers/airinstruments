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


PatternManager::Status PatternManager::SavePattern(void)
{
	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern = drums.getPattern();
	jassert(pattern.get() != nullptr);
	if (!pattern->GetDirty())
		return kCancelled;
	if ((pattern->GetFile() == File::nonexistent) || !pattern->GetModifiable())
		return SavePatternAs();

	String fileName = pattern->GetName();
	fileName = File::createLegalFileName(fileName);
	// For now, use the default path
	File directory = GetDefaultPath();

	DrumPattern::Status saveStatus = pattern->SaveToXml(fileName, directory);
	if (saveStatus != DrumPattern::kNoError)
		return kSaveError;

	return kNoError;
}


PatternManager::Status PatternManager::SavePatternAs(void)
{
	AirHarpApplication::MainWindow* mainWindow = AirHarpApplication::getInstance()->GetMainWindow();
	jassert(mainWindow != nullptr);

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
	SharedPtr<DrumPattern> newPattern(new DrumPattern(*pattern.get()));	// Make a clone of the pattern to be saved with the new name
    newPattern->SetModifiable(true);
	pattern->RevertToClean();	// Undo all changes in original pattern
	Uuid newUuid;	// Create a new Uuid for this new pattern
    newPattern->SetUuid(newUuid);
	DrumPattern::Status saveStatus = newPattern->SaveToXml(fileName, directory);
	if (saveStatus != DrumPattern::kNoError)
		return kSaveError;

	Status buildStatus = BuildPatternList();	// Refresh list to find new content, etc.
	if (buildStatus != kNoError)
		return buildStatus;

	drums.setPattern(newPattern);

	UpdatePrefsLastPattern(newPattern);
	UpdatePatternWheel();

	return kNoError;
}


PatternManager::Status PatternManager::LoadPattern(void)
{
	SaveDirtyPatternPrompt();

	AirHarpApplication::MainWindow* mainWindow = AirHarpApplication::getInstance()->GetMainWindow();
	jassert(mainWindow != nullptr);

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

	UpdatePrefsLastPattern(pattern);
	UpdatePatternWheel();

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
	SaveDirtyPatternPrompt();

	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern(new DrumPattern);
	pattern->SetModifiable(true);
	String patName = "untitled ";
	String kitName = drums.getDrumKit()->GetName();
	patName += kitName;
	pattern->SetName(patName); 
	AddItem(pattern);
	drums.setPattern(pattern);

	UpdatePrefsLastPattern(pattern);
	UpdatePatternWheel();

	return kNoError;
}


void PatternManager::UpdatePrefsLastPattern(SharedPtr<DrumPattern> pattern)
{
    String name = pattern->GetName();
    Uuid uuid = pattern->GetUuid();
    String uuidString = uuid.toString();
    AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	PropertiesFile* propsFile = props.getUserSettings();
	propsFile->setValue("patternName", name);
    propsFile->setValue("patternUuid", uuidString);
}


void PatternManager::UpdatePatternWheel(void)
{
	AirHarpApplication::MainWindow* mainWindow = AirHarpApplication::getInstance()->GetMainWindow();
	jassert(mainWindow != nullptr);

    AirHarpApplication::PatternAddedMessage* m = new AirHarpApplication::PatternAddedMessage;
    ((MainContentComponent*)mainWindow->getContentComponent())->postMessage(m);
}


PatternManager::Status PatternManager::SaveDirtyPatternPrompt(void)
{
	Drums& drums = Drums::instance();
	SharedPtr<DrumPattern> pattern = drums.getPattern();
	jassert(pattern.get() != nullptr);
	if (!pattern->GetDirty())
		return kCancelled;

	AirHarpApplication::MainWindow* mainWindow = AirHarpApplication::getInstance()->GetMainWindow();
	jassert(mainWindow != nullptr);

	int dlgStatus;
	{	// Limit scope, so alert dialog destructs prior to save
		AlertWindow alert("Save modified pattern", "The current pattern is modified.\r\nDo you wish to save it before proceeding ?", AlertWindow::QuestionIcon, mainWindow);
		alert.addButton("Yes", 1, KeyPress(KeyPress::returnKey));
		alert.addButton("No", 0, KeyPress(KeyPress::escapeKey));
		dlgStatus = alert.runModalLoop();
	}

	Status saveStatus;
	if (dlgStatus == 1)
		saveStatus = SavePattern();
	else
		saveStatus = kNoError;

	if ((dlgStatus == 0) || (saveStatus == kCancelled))
	{
		pattern->RevertToClean();
		return kCancelled;
	}

	return saveStatus;
}

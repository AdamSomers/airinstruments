#include "Main.h"
#include "MainMenu.h"
#include "Drums.h"


MainMenu::MainMenu()
{
	AirHarpApplication* app = AirHarpApplication::getInstance();
	ApplicationProperties& props = app->getProperties();
	if (props.getUserSettings()->getBoolValue("tempoSource", Drums::kGlobalTempo) != Drums::kGlobalTempo)
		mUsePatternTempo = true;
	else
		mUsePatternTempo = false;
}


MainMenu::~MainMenu()
{
}


StringArray  MainMenu::getMenuBarNames ()
{
	StringArray	menuBarNames;

	menuBarNames.add("File");
	menuBarNames.add("Options");

	return menuBarNames;
}


PopupMenu  MainMenu::getMenuForIndex (int topLevelMenuIndex, const String& /*menuName*/)
{
	PopupMenu menu;

	switch (topLevelMenuIndex)
	{
		default :
		{
			jassertfalse;
			break;
		}

		case kFileMenu :
			menu.addItem(kLoadPatternCmd, "Load Pattern...");
			menu.addItem(kSavePatternAsCmd, "Save Pattern As...");
			menu.addItem(kExportCmd, "Export Pattern...");
			break;
		case kOptionsMenu :
			menu.addItem(kUsePatternTempoCmd, "Use Pattern Tempo", true, mUsePatternTempo);
			menu.addItem(kAudioSettingsCmd, "Audio Settings...");
			break;
	}


	return menu;
}


void  MainMenu::menuItemSelected (int menuItemID, int/* topLevelMenuIndex*/)
{
	jassert(topLevelMenuIndex <= kLastMenu);
	jassert(topLevelMenuIndex >= 0);

	switch (menuItemID)
	{
		default :
		{
			jassertfalse;
			break;
		}

		case kUsePatternTempoCmd :
		{
			mUsePatternTempo = !mUsePatternTempo;
			// Fall through
		}
		case kAudioSettingsCmd :
		case kSavePatternAsCmd :
		case kLoadPatternCmd :
		case kExportCmd :
		{
			ApplicationCommandTarget::InvocationInfo info(menuItemID);
			info.commandFlags = 0;
			info.invocationMethod = ApplicationCommandTarget::InvocationInfo::InvocationMethod::fromMenu;
			info.originatingComponent = nullptr;
			info.isKeyDown = false;
			info.millisecsSinceKeyPressed = 0;
			JUCEApplication::getInstance()->perform(info);
			break;
		}
	}
}


bool MainMenu::GetUsePatternTempo(void)
{
	return mUsePatternTempo;
}

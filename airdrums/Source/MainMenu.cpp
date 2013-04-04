#include "MainMenu.h"


MainMenu::MainMenu()
{
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
			break;
		case kOptionsMenu :
			menu.addItem(kAudioSettingsCmd, "Audio Settings...");
			break;
	}


	return menu;
}


void  MainMenu::menuItemSelected (int menuItemID, int topLevelMenuIndex)
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

		case kAudioSettingsCmd :
		case kSavePatternAsCmd :
		case kLoadPatternCmd :
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

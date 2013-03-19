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

	menuBarNames.add("Options");

	return menuBarNames;
}


PopupMenu  MainMenu::getMenuForIndex (int topLevelMenuIndex, const String& /*menuName*/)
{
	jassert(topLevelMenuIndex == 0);	// We only have one main menu option currently

	PopupMenu menu;

	menu.addItem(1, "Audio Settings");

	return menu;
}


void  MainMenu::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
	jassert(topLevelMenuIndex == 0);	// We only have one main menu option currently

	switch (menuItemID)
	{
		default :
			jassert(false);
			break;

		case 1 :
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

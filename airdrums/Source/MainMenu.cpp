#include "Main.h"
#include "MainMenu.h"
#include "Drums.h"


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
    ApplicationCommandManager* mgr = &(AirHarpApplication::getInstance()->commandManager);
	switch (topLevelMenuIndex)
	{
		default :
		{
			jassertfalse;
			break;
		}

		case kFileMenu :
        {
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kNewPatternCmd, "New Pattern");
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kLoadPatternCmd, "Load Pattern...");
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kSavePatternCmd, "Save Pattern");
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kSavePatternAsCmd, "Save Pattern As...");
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kExportCmd, "Export Pattern...");
            menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kDeletePatternCmd);
            menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kMoreInfoCmd, "More Info...");
            
			break;
        }
		case kOptionsMenu :
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kUsePatternTempoCmd, "Use Pattern Tempo");
			menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kAudioSettingsCmd, "Audio Settings...");
            menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kFullscreenCmd, "Toggle Fullscreen Mode");
            menu.addCommandItem(mgr, AirHarpApplication::MainWindow::kAdvancedModeCmd, "Toggle Advanced Mode");
			break;
	}


	return menu;
}


void  MainMenu::menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/)
{
}

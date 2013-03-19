#ifndef h_MainMenu
#define h_MainMenu

#include "../JuceLibraryCode/JuceHeader.h"

// This is the main menu bar for the AirDrums app

class MainMenu : public MenuBarModel
{
public:
    MainMenu();
    virtual ~MainMenu();

	virtual StringArray  getMenuBarNames ();
	virtual PopupMenu  getMenuForIndex (int topLevelMenuIndex, const String &menuName);
	virtual void  menuItemSelected (int menuItemID, int topLevelMenuIndex);

private:
};

#endif

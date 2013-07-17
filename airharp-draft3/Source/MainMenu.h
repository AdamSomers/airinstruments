#ifndef h_MainMenu
#define h_MainMenu

#include "../JuceLibraryCode/JuceHeader.h"

// This is the main menu bar for the AirBeats app

class MainMenu : public MenuBarModel
{
public:
	enum Command
	{
		
	};
	enum TopLevelMenu
	{
		kFileMenu = 0,
		kOptionsMenu = 1,
        
		kLastMenu = kOptionsMenu
	};
    
    MainMenu();
    virtual ~MainMenu();
    
	virtual StringArray  getMenuBarNames ();
	virtual PopupMenu  getMenuForIndex (int topLevelMenuIndex, const String &menuName);
	virtual void  menuItemSelected (int menuItemID, int topLevelMenuIndex);
    
private:
};

#endif

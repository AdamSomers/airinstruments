#ifndef h_MainMenu
#define h_MainMenu

#include "../JuceLibraryCode/JuceHeader.h"

// This is the main menu bar for the AirBeats app

class MainMenu : public MenuBarModel
{
public:
	enum Command
	{
		kAudioSettingsCmd = 1,
		kSavePatternAsCmd = 2,
		kLoadPatternCmd = 3,
		kUsePatternTempoCmd = 4,
		kExportCmd = 5,
		kNewPatternCmd = 6,
		kSavePatternCmd = 7
	};
	enum TopLevelMenu
	{
		kFileMenu = 0,
		kOptionsMenu = 1,

		kLastMenu = kOptionsMenu
	};

    MainMenu();
    virtual ~MainMenu();

	bool GetUsePatternTempo(void);

	virtual StringArray  getMenuBarNames ();
	virtual PopupMenu  getMenuForIndex (int topLevelMenuIndex, const String &menuName);
	virtual void  menuItemSelected (int menuItemID, int topLevelMenuIndex);

private:
	bool	mUsePatternTempo;
};

#endif

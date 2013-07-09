AirBeats - Release Notes

Please send all comments to adam@handwavy.co.  Thanks!
Diagnostic Logs are written ~/Library/Logs/AirBeats (Mac), and C:\Users\<user>\App Data\Roaming\AirBeats (Win).

# AirBeats
Make awesome sounding beats in the air!  AirBeats captures your natural "air drumming" movements, allowing you to make amazing beats in the styles of Hip Hop, Dubstep, Trance, and more.

## Features & Usage
- "Air drum" over the Leap Motion Controller to play the six zones.
- The application detects rapid downward movement of your hands.  You can play in a variety of ways: open hands, finger extended, or using tools.  Pencils and chopsticks recommended!
- When not drumming, a cursor tacks where you are pointing to on the screen (use one finger or tool to move the cursor).
- All buttons are activated by hovering the cursor over the button.
- Select kits and patterns by bringing the lists into view with the tabs.  Point at the list items to make your selection.
- Change the tempo using the left/right buttons on either side of the tempo display.
- Control the transport (play/record/metronome) using the transport buttons in the upper-right.
- An audio settings dialog is available in the Options menu, in case you need to use a different audio device.
- Patterns can be saved, copied, and deleted from the file menu.
- You can export your pattern to an audio file from the file menu.
- If you dismiss the tutorial with the "Don't show again" button, you can bring it back with the 'h' key on your keyboard.

## Tips
- Windows users should use an ASIO audio driver for the lowest possible latency.
- It it helpful to view the Leap Motion Visualizer along side AirBeats to get a feel for the playable zones and proper motions.

## System Requirements
- Windows 7 or 8
- AMD Phenom™ II or Intel® Core™ i3, i5, i7 processor
- 2 GB RAM
- 150 MB HDD space
- OpenGL 3.0 capable graphics hardware

v0.8 - 7/9/2013
==================

What's New

Resolved the following issues:

ISSUE 2970 - User is unable to delete patterns from the patterns list
ISSUE 2968 - Hip-Hop 1 is listed after Hip-Hop 2 in Kits list
ISSUE 2967 - If multiple instances of the same drum are assigned, when 1 is highlighted, they are all highlighted

Completed features: 

- Make pad highlight more obvious
- Filter repeated notes in case of jumpy tracking/detection
- Improve detection of pointable with no palm present
- Fix support for playing with closed fists
- Add key commands to control menu items from keyboard
- Disable unavailable menu options such as save/delete on a factory pattern


v0.7.2 - 7/6/2013
==================

Resolved the following issues:

ISSUE 3393 - App contains a timer, which causes the user to close the app upon expiration
ISSUE 3389 - App references Leap Motion Controller incorrectly on the 2nd tutorial screen

v0.7.1 - 6/24/2013
==================

Update to Leap SDK 0.8.0


v0.7 - 6/19/2013
==================

Resolved the following issues:

ISSUE 2672 - App does not inform the user when the Leap Motion Controller is disconnected or software is not running.
ISSUE 2691 - Some pads cannot be hit using the Leap Motion Controller.
ISSUE 2685 - Minimizing and then Restoring the window causes the screen to go black.
ISSUE 2683 - Using small objects such as pencils do not function well.
ISSUE 2651 - User is able lower the BPM to a negative number.
ISSUE 2679 - The tutorial only covers the playing of the set pads, there is no mention of all the other functions.
ISSUE 2649 - "Clear" text appears over "Assign" text when first starting App.
ISSUE 2647 - There is a selectable Kit that has no label and several unselectable Kit spots.

Other changes in this release

- Display whether app is in "pointing" or "playing" mode.
- Add dots to tutorial to indicate progress.

These issues require further information:

ISSUE 2693 - User cannot close the App from the taskbar (Windows).

    Not seeing this on my system, Win 7 64Bit in Parallels 7 on 2008 Mac Pro,
    Win 7 64Bit 2008 MacBook Pro Boot Camp. Please provide system specs.
    Context menu and icon "x" button work as expected.

ISSUE 2650 - Strange line through images (Windows).

    Not seeing this on my machine Win 7 64Bit in Parallels 7, Win 7 64Bit on
    2008 MacBook Pro Bootcamp. Please provide screenshots and system specs.

Responses to recommendations:

FEEDBACK 2657 - Several drum icons are identical.

    Further labeling is possible but would detract from the minimalist
    aesthetic. Some commercial drum machine apps display the wave file name on
    the pad, and some display no iconography whatsoever (just blank pads). My
    hope is that the kits' specific layout will be committed to memory as the
    user masters the app. If this turns out to be a problem for users I will
    either resort to labeling or some programmatic variation of drum icons.
    The app is capable of custom icons for each and every sound, which would
    be the optimal solution given ample time & resources.

FEEDBACK 2653 - User should be able to increase and decreas the BPM using the keyboard.

    Agreed, and planned for a future release.

FEEDBACK 2684 - It would be a nice feature if the user could have more items assigned at once.

    Six was determined to be maximum number of pads that could be easily
    accessed given the Leap Motion Controller's field of a view. I like the
    idea of switching to alternate sets of items, though using open/closed
    hand for this presents interaction collisions (the current set of pads
    should be playable with closed hands, open hands, or tools). A button or
    gesture to pan to the next set of icons would be ideal. The current drum
    icon row can be repurposed to facilitate this, though I would keep the
    random-access select-ability as an 'advanced' option. Note that this is a
    low priority enhancement and probably won't make it in until a future
    release, post-shipment.

FEEDBACK 2674 - It would be useful to show an icon on screen so that the user can see which pads they will hit.

    Currently the pad lights up slightly when the hand is hovering over it. I
    plan to create an alternate indication of this to a) make it more obvious
    and b) better distinguish from the trigger light up animation.

FEEDBACK 2655 - Drum icons do not have labels unless assigned.

    I agree that icons alone could bewilder novice users, so I plan to add
    labels over the drum icons during the hover state. This will at least
    provide information about the icon's identity during the assignment
    operation. Note also that the icons and their labels will be documented in
    a help file. Further labeling will add clutter that detracts from the
    minimalist aesthetic. The drum icons serve as a visual identity for each
    drum sound, which should suffice once the user has interacted with the app
    for some time. However,

==================
v0.6 - 6/12/2013

Completed features:

- Pads are now laid out in a tilted 3D view to better indicate the Z-depth interaction space.
- Linear lists instead of 'wheels'
- New Pattern function
- Management of user vs. factory patterns: Factory patterns are stored with the application bundle and cannot be overwritten.  A modified factory pattern can only be saved-as.
- Editable patterns (can be overwritten with a standard "save" function).
- Navigating away from a modified pattern prompts for save/discard.
- Improved cursor tracking: cursor position is smoothed, and cursor will persist when tracking is flickery.  Note that a reasonable pointing posture must be used so that the pointing finger is within the Leap's field of view.
- Removed layout switching.
- Added diagnostic data logging to file.  On Mac, logs are written to ~/Library/Logs/AirBeats.  On Win, logs are written to C:\Users\<user>\App Data\Roaming\AirBeats.
- Disabled drum playing on first screen of tutorial.
- Fixed tutorial layout when window size changes.

==================
v0.5 - 5/20/2013

Completed features:

- Timeline visualization
- Relocate transport controls to center
- More minimal look for tempo control
- Added Analog Hip Hop kit

Currently in development:

- Linear lists instead of 'wheels'
- "New Pattern" function
- Management of user vs. factory patterns

==================
v0.4 - 5/17/2013

Completed features:

- New instrument icon set
- Multiple strike zone layouts
- Color-code icons to pads
- New hover+drag drum assignment method
- New clear track method
- Optimized graphics for Intel HD cards (fixes MacBook Air issues)
- Splash screen
- Tutorial mode on app launch
- Integrated professionally designed drumkits

Currently in development:

- Timeline visualization
- Linear lists instead of 'wheels'
- Relocate transport controls to center
- More minimal look for tempo control
- "New Pattern" function
- Management of user vs. factory patterns

==================
v0.3 - 4/26/2013

Completed features:

- New drumming interaction. Very responsive to many different playing styles.
- Velocity sensitivity
- Cursor
- Buttons with hover timeout and animation
- Removed gesture input
- Removed finger visualization
- Random-access to drum sounds.  Drum selector no longer a carousel.
- Clear track
- Clear pattern
- Resolved issue wherein wheel selections can become jumbled
- Resolved issue wherein changing pattern did not update kit wheel

Currently in development:

- Continued trigger accuracy improvements
- Tutorial mode with instructional overlays
- New icons for drum graphics
- display drum graphic and name on trigger pad
- Timeline visualization
- Professionally designed drum kits and default patterns
- Splash screen and initialization progress indication 

Known issues:

- If a large number of patterns are saved, the pattern wheel is overcrowded.  Need to set a maximum 

==================
v0.2 - 4/18/2013

Completed features:

- Tempo Control
- Kit selection control
- Pattern selection control
- Professionally designed graphics

Currently in development:

- Customized tapping gesture that allows for more natural drumming motions
- Simplified finger visualization
- Simplified interaction with selectors
- Alternate transport control interaction
- Tutorial mode with instructional overlays
- Clear track
- Professionally designed drum kits and default patterns
- Splash screen and initialization progress indication 

Known issues:

- When pattern changes kit or tempo, those selectors don't update.
- Kit selector items can get jumbled.  They return to normal when the control is dismissed.
- If a large number of patterns are saved, the pattern wheel is overcrowded.  Need to set a maximum 

==================
v0.1 - 4/4/2013

Completed features:

- Tap to trigger
- Transport gestures
- Drum Selector UI
- Trig View UI
- Drum Kit XML definition
- Load Drum kits
- Display standard iconography for sound categories
- Display custom icons for sounds when specified in XML
- Save/Load patterns
- Audio settings (device, sample rate, buffer size)
- Play Area UI
- Persist settings (L/R selection, kit)

Currenty in development:

- Tutorial mode with instructional overlays
- Tempo Control
- Kit selection control
- Pattern selection control
- Clear track
- Professionally designed graphics
- Professionally designed drum kits and default patterns

Other planned v1 features:

- Audio export
- Motion-controlled dialogs to verify certain actions (clear pattern, overwrite pattern, missing data, etc)
- Change finger graphics
- Animated effect on tap
- Velocity layers
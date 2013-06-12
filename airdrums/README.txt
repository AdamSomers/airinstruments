AirBeats - Release Notes

Please send all comments to adam@handwavy.co.  Thanks!
Diagnostic Logs are written ~/Library/Logs/AirBeats (Mac), and C:\Users\<user>\App Data\Roaming\AirBeats (Win).

Usage:

- "Air drum" in over the Leap's field of view to play one of six zones.

- The application detects rapid downward movement of your hands.  You can play in a variety of ways: open hands, finger extended, closed fist (imaginary drumstick), or using tools.

- When not drumming, a cursor tacks where you are pointing to on the screen (use one finger or tool to move the cursor).

- The pads illuminate as you wave your hands over them.

- All buttons are activated by hovering the cursor over the button.

- Select kits and patterns by bringing the lists into view with the arrow buttons.  Point at a list item to make your selection.

- Change the tempo using the left/right buttons on either side of the tempo display.

- Control the transport (play/record/metronome) using the transport buttons in the upper-right.

- An audio settings dialog is available in the Options menu, in case you need to use a different audio device.

- Patterns can be saved from the file menu.

- You can export your pattern to an audio file from the file menu.

- If you dismiss the tutorial with the "Don't show again" button, you can bring it back with the 'h' key on your keyboard.


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
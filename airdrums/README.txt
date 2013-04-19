AirBeats - Release Notes

Note: The application works best when used with one finger pointing from each hand.  The fact that all fingers are visualized leads to easy confusion, and will be addressed in a future build.  The current plan is to switch to using the screen API to display a cursor, of which there will only be one.

Also take note that all kits have the same placeholder sounds, except HipHop which has different placeholder sounds.

Please send all comments to adam@handwavy.co.  Thanks!

Usage:

- Tap on the left or right half of the Leap's field of view to play the L or R sound.

- Hover over the drum selectors to change the L or R sound.  The tracked finger's distance from the horizontal center of the control determines the rate at which the drum selection advances. 

- Hover over the wheel "tabs" on the left and right edges of the screen to bring out the kit and pattern wheels.  The tracked finger's distance from the vertical center of the wheel determines the rate at which the selection advances.  The wheels are automatically dismissed when you stop interacting with them.

- Tempo control works similarly to drum selector.

Transport controls:
- One clockwise circle gesture starts the transport
- Two clockwise circles toggles record mode
- Three clockwise circles toggles the metronome
- One counter-clockwise circle stops the transport
- Two counter-clockwise circles resets the transport to start of pattern
- Three counter-clockwise circles clears the pattern

An audio settings dialog is available in the Options menu, in case you need to use a different audio device.

Patterns can be saved from the file menu.

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
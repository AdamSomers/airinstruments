/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  10 May 2013 6:10:03pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_EXPORTCOMPONENT_EXPORTCOMPONENT_1AB84337__
#define __JUCER_HEADER_EXPORTCOMPONENT_EXPORTCOMPONENT_1AB84337__

//[Headers]     -- You can add your own extra header files here --
#include "Main.h"
#include "Drums.h"
#include "../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ExportComponent  : public Component,
                         public TextEditor::Listener,
                         public ButtonListener
{
public:
    //==============================================================================
    ExportComponent ();
    ~ExportComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void SetParentDialog(DialogWindow* parent);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	enum State
	{
		kStateIdle = 0,
		kStateExporting = 1,
		kStateCancelled = 2,
		kStateCompleted = 3,
		kStateError = 4
	};

	void Export(void);
	File GetFileName(void);
	void EnsureUniqueName(void);
	void EnsureLegalName(void);
	void FolderBrowse(void);

	void textEditorFocusLost(TextEditor& editor);
	void textEditorTextChanged(TextEditor& editor);

	double mProgressValue;
	volatile State mState;
	String mOriginalName;
	bool mNameChanged;
	DialogWindow* mParent;
	AirHarpApplication* mApp;
	Drums* mDrums;
	Drums::TransportState* mTransport;
    //[/UserVariables]

    //==============================================================================
    TextEditor* mFileNameEdit;
    Label* mFileNameLabel;
    TextEditor* mFolderEdit;
    Label* mFolderLabel;
    TextButton* mBrowseButton;
    ProgressBar* mProgressBar;
    TextButton* mExportButton;
    TextButton* mCancelButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExportComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCER_HEADER_EXPORTCOMPONENT_EXPORTCOMPONENT_1AB84337__

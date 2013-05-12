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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "ExportComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
#define	kExportFolderProperty	"exportFolder"
//[/MiscUserDefs]

//==============================================================================
ExportComponent::ExportComponent ()
    : mFileNameEdit (0),
      mFileNameLabel (0),
      mFolderEdit (0),
      mFolderLabel (0),
      mBrowseButton (0),
      mProgressBar (0),
      mExportButton (0),
      mCancelButton (0)
{
    addAndMakeVisible (mFileNameEdit = new TextEditor ("file name edit"));
    mFileNameEdit->setMultiLine (false);
    mFileNameEdit->setReturnKeyStartsNewLine (false);
    mFileNameEdit->setReadOnly (false);
    mFileNameEdit->setScrollbarsShown (false);
    mFileNameEdit->setCaretVisible (true);
    mFileNameEdit->setPopupMenuEnabled (false);
    mFileNameEdit->setText (String::empty);

    addAndMakeVisible (mFileNameLabel = new Label ("file name label",
                                                   "Export file name"));
    mFileNameLabel->setFont (Font (15.0000f, Font::plain));
    mFileNameLabel->setJustificationType (Justification::centredLeft);
    mFileNameLabel->setEditable (false, false, false);
    mFileNameLabel->setColour (TextEditor::textColourId, Colours::black);
    mFileNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (mFolderEdit = new TextEditor ("folder edit"));
    mFolderEdit->setMultiLine (false);
    mFolderEdit->setReturnKeyStartsNewLine (false);
    mFolderEdit->setReadOnly (true);
    mFolderEdit->setScrollbarsShown (false);
    mFolderEdit->setCaretVisible (false);
    mFolderEdit->setPopupMenuEnabled (false);
    mFolderEdit->setText (String::empty);

    addAndMakeVisible (mFolderLabel = new Label ("folder label",
                                                 "Export folder"));
    mFolderLabel->setFont (Font (15.0000f, Font::plain));
    mFolderLabel->setJustificationType (Justification::centredLeft);
    mFolderLabel->setEditable (false, false, false);
    mFolderLabel->setColour (TextEditor::textColourId, Colours::black);
    mFolderLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (mBrowseButton = new TextButton ("browse button"));
    mBrowseButton->setButtonText ("Browse");
    mBrowseButton->addListener (this);

    addAndMakeVisible (mProgressBar = new ProgressBar (mProgressValue));
    mProgressBar->setName ("progress bar");

    addAndMakeVisible (mExportButton = new TextButton ("export button"));
    mExportButton->setButtonText ("Export");
    mExportButton->addListener (this);

    addAndMakeVisible (mCancelButton = new TextButton ("cancel button"));
    mCancelButton->setButtonText ("Cancel");
    mCancelButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (700, 200);


    //[Constructor] You can add your own custom stuff here..
	mApp = AirHarpApplication::getInstance();
	mDrums = &Drums::instance();
	mTransport = &mDrums->getTransportState();

	mTransport->pause();

	mProgressValue = 0.0;
	mState = kStateIdle;
	mParent = nullptr;
	mNameChanged = false;

	File folder;
	String lastFolder = mApp->getProperties().getUserSettings()->getValue(kExportFolderProperty, "");
	if (lastFolder != "")
		folder = lastFolder;
	else
	{
		folder = File::getSpecialLocation(File::userDocumentsDirectory);
		folder = folder.getChildFile("AirBeats Exports");
	}
	if (!folder.isDirectory())
	{
		Result result = folder.createDirectory();
		if (result.failed())
			jassertfalse;
	}
	mFolderEdit->setText(folder.getFullPathName());
	mOriginalName = mDrums->getPattern()->GetName();
	mFileNameEdit->setText(mOriginalName);
	EnsureUniqueName();
	mFileNameEdit->addListener(this);
    //[/Constructor]
}

ExportComponent::~ExportComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (mFileNameEdit);
    deleteAndZero (mFileNameLabel);
    deleteAndZero (mFolderEdit);
    deleteAndZero (mFolderLabel);
    deleteAndZero (mBrowseButton);
    deleteAndZero (mProgressBar);
    deleteAndZero (mExportButton);
    deleteAndZero (mCancelButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ExportComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ExportComponent::resized()
{
    mFileNameEdit->setBounds (160, 16, 352, 24);
    mFileNameLabel->setBounds (24, 16, 120, 24);
    mFolderEdit->setBounds (160, 56, 352, 24);
    mFolderLabel->setBounds (24, 56, 120, 24);
    mBrowseButton->setBounds (528, 56, 102, 24);
    mProgressBar->setBounds (32, 104, 600, 24);
    mExportButton->setBounds (88, 160, 150, 24);
    mCancelButton->setBounds (424, 160, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ExportComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == mBrowseButton)
    {
        //[UserButtonCode_mBrowseButton] -- add your button handler code here..
		if (mState == kStateIdle)
		{
			FolderBrowse();
		}
        //[/UserButtonCode_mBrowseButton]
    }
    else if (buttonThatWasClicked == mExportButton)
    {
        //[UserButtonCode_mExportButton] -- add your button handler code here..
		if (mState == kStateIdle)
		{
			mState = kStateExporting;
			Export();
		}
        //[/UserButtonCode_mExportButton]
    }
    else if (buttonThatWasClicked == mCancelButton)
    {
        //[UserButtonCode_mCancelButton] -- add your button handler code here..
		if (mState == kStateIdle)
		{
			mState = kStateCancelled;
			jassert(mParent != nullptr);
			mParent->exitModalState(0);
		}
		else
		if (mState == kStateExporting)
		{
			mState = kStateCancelled;
		}
        //[/UserButtonCode_mCancelButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ExportComponent::textEditorFocusLost(TextEditor& editor)
{
	if ((&editor == mFileNameEdit) && mNameChanged)
	{
		mNameChanged = false;
		EnsureLegalName();
		EnsureUniqueName();
	}
}


void ExportComponent::textEditorTextChanged(TextEditor& editor)
{
	if ((&editor == mFileNameEdit) && mFileNameEdit->hasKeyboardFocus(true))
		mNameChanged = true;
}


void ExportComponent::SetParentDialog(DialogWindow* parent)
{
	mParent = parent;
}


File ExportComponent::GetFileName(void)
{
	File folder(mFolderEdit->getText());
	File file = folder.getChildFile(mFileNameEdit->getText() + ".wav");
	return file;
}


void ExportComponent::EnsureUniqueName(void)
{
	File folder(mFolderEdit->getText());
	File file = folder.getNonexistentChildFile(mOriginalName, ".wav", true);
	mFileNameEdit->setText(file.getFileNameWithoutExtension());
}


void ExportComponent::EnsureLegalName(void)
{
	String legalName = File::createLegalFileName(mFileNameEdit->getText());
	mFileNameEdit->setText(legalName);
	mOriginalName = legalName;
}


void ExportComponent::FolderBrowse(void)
{
	FileChooser chooser("Select Export Folder", File(mFolderEdit->getText()));
	if (!chooser.browseForDirectory())
		return;
	File folder = chooser.getResult();
	mFolderEdit->setText(folder.getFullPathName());
	mApp->getProperties().getUserSettings()->setValue(kExportFolderProperty, mFolderEdit->getText());
	EnsureUniqueName();
}


#if defined(_WIN32)
	// Suppress warning C4127: conditional expression is constant for the while (true)
	#pragma warning(push)
	#pragma warning(disable : 4127)
#endif

void ExportComponent::Export(void)
{
	File file = GetFileName();
	FileOutputStream* stream = new FileOutputStream(file);
	WavAudioFormat format;
	UniquePtr<AudioFormatWriter> writer(format.createWriterFor(stream, mDrums->getSampleRate(), 2, 32, StringPairArray(), 0));
	jassert(writer.get() != nullptr);
	const long blockSize = 2048;

	mDrums->AllNotesOff();
	mApp->StopAudioDevice();
	mDrums->resetToZero();

	long totalSamples = mDrums->getMaxSamples();
	long samplesToGo = totalSamples;
	long samplesCompleted = 0;
	mTransport->doExport();
	mTransport->play();
	while (true)
	{
		if (mState == ExportComponent::kStateCancelled)
			break;
		if (samplesToGo <= 0)
		{
			mState = kStateCompleted;
			break;
		}
		long count = jmin(samplesToGo, blockSize);
		samplesToGo -= count;
		samplesCompleted += count;
		bool status = writer->writeFromAudioSource(*mDrums, count, count);
		if (!status)
		{
			// May need an error notification of some kind here
			jassertfalse;
			mState = kStateError;
			break;
		}
		mProgressValue = ((double) samplesCompleted) / ((double) totalSamples);
	}
	mTransport->pause();
	mDrums->resetToZero();
	mDrums->AllNotesOff();

	mApp->StartAudioDevice();

	jassert(mParent != nullptr);
	mParent->exitModalState(mState == kStateCompleted ? 1 : 0);
}

#if defined(_WIN32)
	#pragma warning(pop)
#endif
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ExportComponent" componentName=""
                 parentClasses="public Component, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="1" initialWidth="700"
                 initialHeight="200">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TEXTEDITOR name="file name edit" id="3192c88951eafc67" memberName="mFileNameEdit"
              virtualName="" explicitFocusOrder="0" pos="160 16 352 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="0"
              caret="1" popupmenu="0"/>
  <LABEL name="file name label" id="2d799561b3938d6a" memberName="mFileNameLabel"
         virtualName="" explicitFocusOrder="0" pos="24 16 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Export file name" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="folder edit" id="4efc292f11333663" memberName="mFolderEdit"
              virtualName="" explicitFocusOrder="0" pos="160 56 352 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="1" scrollbars="0"
              caret="0" popupmenu="0"/>
  <LABEL name="folder label" id="15e50c1cb3489c35" memberName="mFolderLabel"
         virtualName="" explicitFocusOrder="0" pos="24 56 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Export folder" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="browse button" id="4a2b052ec8baf755" memberName="mBrowseButton"
              virtualName="" explicitFocusOrder="0" pos="528 56 102 24" buttonText="Browse"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="progress bar" id="6e6dddac00c18a26" memberName="mProgressBar"
                    virtualName="" explicitFocusOrder="0" pos="32 104 600 24" class="ProgressBar"
                    params="mProgressValue"/>
  <TEXTBUTTON name="export button" id="b5a1c103b45fa546" memberName="mExportButton"
              virtualName="" explicitFocusOrder="0" pos="88 160 150 24" buttonText="Export"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="cancel button" id="3640effaa57eacaa" memberName="mCancelButton"
              virtualName="" explicitFocusOrder="0" pos="424 160 150 24" buttonText="Cancel"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[EndFile] You can add extra defines here...
//[/EndFile]

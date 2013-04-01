/*
  ==============================================================================

    PatternSaveDialog.cpp
    Created: 1 Apr 2013 10:44:19am
    Author:  Bob Nagy

  ==============================================================================
*/

#include "PatternSaveDialog.h"


PatternSaveDialog::PatternSaveDialog(DocumentWindow* window)
    : DialogWindow ("Save Pattern As", Colours::azure, false, true)
	, mListener(this)
{
	mEditor.setWantsKeyboardFocus(true);
	mEditor.addListener(&mListener);
	mEditor.setSize(300, 30);

	setWantsKeyboardFocus(false);
    setUsingNativeTitleBar (true);
    setContentNonOwned (&mEditor, true);
    centreAroundComponent (window, getWidth(), getHeight());
    setResizable (false, false);

	mEditor.setSize(300, 30);
}


PatternSaveDialog::~PatternSaveDialog()
{
}


void PatternSaveDialog::closeButtonPressed()
{
    setVisible (false);
}


String PatternSaveDialog::GetFileName(void)
{
	return mEditor.getText();
}


PatternSaveDialog::Listener::Listener(DialogWindow* dlg) :
	mDlg(dlg)
{
}


PatternSaveDialog::Listener::~Listener()
{
}


void PatternSaveDialog::Listener::textEditorReturnKeyPressed(TextEditor&)
{
	jassert(mDlg != nullptr);
	mDlg->exitModalState(1);
}


void PatternSaveDialog::Listener::textEditorEscapeKeyPressed(TextEditor&)
{
	jassert(mDlg != nullptr);
	mDlg->exitModalState(0);
}

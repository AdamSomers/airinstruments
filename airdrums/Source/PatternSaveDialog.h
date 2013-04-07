/*
  ==============================================================================

    PatternSaveDialog.h
    Created: 1 Apr 2013 10:44:32am
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __PATTERNSAVEDIALOG_H_94235434__
#define __PATTERNSAVEDIALOG_H_94235434__

#include "../JuceLibraryCode/JuceHeader.h"


class PatternSaveDialog : public DialogWindow
{
public:

    PatternSaveDialog(DocumentWindow* window);
	~PatternSaveDialog();

    void	closeButtonPressed();

	String	GetFileName(void);

private:
	TextEditor				mEditor;

	class Listener : public TextEditor::Listener
	{
	public:
		Listener(DialogWindow* dlg);
		~Listener();

	void	textEditorReturnKeyPressed(TextEditor&);
	void	textEditorEscapeKeyPressed(TextEditor&);

	private:
		DialogWindow*	mDlg;
	}	mListener;
};

#endif  // __PATTERNSAVEDIALOG_H_94235434__

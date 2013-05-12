/*
  ==============================================================================

    PatternLoadDialog.h
    Created: 3 Apr 2013 3:13:02pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __PATTERNLOADDIALOG_H_E47EDE9D__
#define __PATTERNLOADDIALOG_H_E47EDE9D__

#if defined(_WIN32)
#include "GL/glew.h"
#endif

#include "../JuceLibraryCode/JuceHeader.h"

class PatternLoadDialog : public DialogWindow
{
public:

    PatternLoadDialog(DocumentWindow* window);
	~PatternLoadDialog();

    void	closeButtonPressed();
	bool	keyPressed(const KeyPress &key);

	int		GetSelection(void);

private:
	ListBox	mListBox;

	class PatternLoadListBoxModel : public ListBoxModel
	{
	public :
		PatternLoadListBoxModel(PatternLoadDialog* dialog, ListBox* listBox);
		~PatternLoadListBoxModel();

		int		getNumRows();
		void	paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected);
		void	listBoxItemDoubleClicked(int row, const MouseEvent &e);
		void	returnKeyPressed(int lastRowSelected);

		int					mSelectedItem;
		ListBox*			mListBox;
		PatternLoadDialog*	mDialog;
	}	mListBoxModel;
};

#endif  // __PATTERNLOADDIALOG_H_E47EDE9D__

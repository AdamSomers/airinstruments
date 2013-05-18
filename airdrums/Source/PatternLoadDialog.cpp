/*
  ==============================================================================

    PatternLoadDialog.cpp
    Created: 3 Apr 2013 3:12:54pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "PatternLoadDialog.h"
#include "PatternManager.h"


PatternLoadDialog::PatternLoadDialog(DocumentWindow* window)
    : DialogWindow ("Load Pattern", Colours::azure, false, true)
	, mListBox("", &mListBoxModel)
	, mListBoxModel(this, &mListBox)
{
	mListBox.setWantsKeyboardFocus(true);
	mListBox.setSize(300, 250);
	mListBox.setBounds(0, 0, 300, 250);

	setWantsKeyboardFocus(false);
    setUsingNativeTitleBar (true);
    setContentNonOwned (&mListBox, true);
    centreAroundComponent (window, getWidth(), getHeight());
    setResizable (false, false);

	mListBox.updateContent();
}


PatternLoadDialog::~PatternLoadDialog()
{
}


void PatternLoadDialog::closeButtonPressed()
{
    setVisible (false);
}


int PatternLoadDialog::GetSelection(void)
{
	return mListBoxModel.mSelectedItem;
}


bool PatternLoadDialog::keyPressed(const KeyPress &key)
{
	if (key == KeyPress::returnKey)
	{
		exitModalState(1);
		return true;
	}
	else
	if (key == KeyPress::escapeKey)
	{
		exitModalState(0);
		return true;
	}
	else
		return false;
}


PatternLoadDialog::PatternLoadListBoxModel::PatternLoadListBoxModel(PatternLoadDialog* dialog, ListBox* listBox) :
	mSelectedItem(-1)
	, mDialog(dialog)
	, mListBox(listBox)
{
}


PatternLoadDialog::PatternLoadListBoxModel::~PatternLoadListBoxModel()
{
}


int PatternLoadDialog::PatternLoadListBoxModel::getNumRows()
{
	PatternManager& mgr = PatternManager::GetInstance();
	return mgr.GetItemCount();
}


void PatternLoadDialog::PatternLoadListBoxModel::paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
	PatternManager& mgr = PatternManager::GetInstance();
	SharedPtr<DrumPattern> pattern = mgr.GetItem(rowNumber);
    if (rowIsSelected)
	{
        g.setColour(Colours::blue);
        g.fillAll();
	    g.setColour(Colours::white);
    }
	else
	{
	    g.setColour(Colours::black);
	}
	juce::Rectangle<int> itemRect(0, 0, width, height);
	g.drawText(pattern->GetName(), itemRect, Justification::topLeft, false);
}


void PatternLoadDialog::PatternLoadListBoxModel::listBoxItemDoubleClicked(int row, const MouseEvent& /*e*/)
{
	mSelectedItem = row;
	mDialog->exitModalState(1);
}


void PatternLoadDialog::PatternLoadListBoxModel::returnKeyPressed(int lastRowSelected)
{
	mSelectedItem = lastRowSelected;
	mDialog->exitModalState(1);
}

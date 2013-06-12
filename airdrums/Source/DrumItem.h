/*
  ==============================================================================

    DrumItem.h
    Created: 30 Mar 2013 12:15:36pm
    Author:  Bob Nagy

  ==============================================================================
*/

#ifndef __DRUMITEM_H_87D00F4E__
#define __DRUMITEM_H_87D00F4E__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Types.h"


// This is the base class from which DrumKit and DrumPattern derive.
template <typename Derived>
class DrumItem
{
public:
	enum Status
	{
		kNoError = 0,
		kItemLoadError = 1,
		kItemEmptyError = 2,
		kItemSaveError = 3,
		kItemNameError = 4,
		kItemUuidError = 5,
		kNoKitError = 6,
		kSampleRateError = 7,
		kSamplePositionError = 8,
		kMidiDataError = 9
	};

	~DrumItem();

	// Other accessors

	const String&	GetName(void) const;
	const Uuid&		GetUuid(void) const;
	const File&		GetFile(void) const;
	void	SetName(const String& name);
	void	SetUuid(const Uuid& uuid);
	void	SetFile(const File& file);

	bool	GetModifiable(void) const;
	void	SetModifiable(bool modifiable);
	bool	GetDirty(void) const;
	void	SetDirty(bool dirty);

	void	PrepareToModify(void);
	void	RevertToClean(void);

	Status	LoadFromXml(XmlElement* element);
	Status	SaveToXml(XmlElement* element);

protected:
	DrumItem();
	DrumItem(const DrumItem& source);

private:
	String					mName;
	Uuid					mUuid;
	File					mFile;			// File object this item was loaded/saved from/to.
	UniquePtr<Derived>		mCleanCopy;		// Copy of this prior to being modified, used to undo changes
	bool					mModifiable;
	bool					mDirty;
};

// I dislike code in header files, so I put all definition in a cpp file and include it here.
// This code needs to be available in the header at compile time since the class is a template.
// This cpp file should not be compiled separately in the build process, but only included here.
#include "DrumItem.cpp"

#endif  // __DRUMITEM_H_87D00F4E__

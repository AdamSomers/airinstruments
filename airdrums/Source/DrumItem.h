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


// This is the base class from which DrumKit and DrumPattern derive.
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

	String&	GetName(void);
	Uuid&	GetUuid(void);
	void	SetName(String& name);
	void	SetUuid(Uuid uuid);

	Status	LoadFromXml(XmlElement* element);
	Status	SaveToXml(XmlElement* element);

protected:
	DrumItem();

private:
	String	mName;
	Uuid	mUuid;
};

#endif  // __DRUMITEM_H_87D00F4E__

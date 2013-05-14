/*
  ==============================================================================

    DrumItem.cpp
    Created: 30 Mar 2013 12:21:52pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumItem.h"


DrumItem::DrumItem() :
	mModifiable(false)
	, mDirty(false)
	, mHasValidName(false)
{
}


DrumItem::DrumItem(const DrumItem& source) :
	mModifiable(true)
	, mDirty(source.GetDirty())
	, mHasValidName(false)
{
}


DrumItem::~DrumItem()
{
}


String& DrumItem::GetName(void)
{
	return mName;
}


Uuid& DrumItem::GetUuid(void)
{
	return mUuid;
}


void DrumItem::SetName(String& name)
{
	mName = name;
	if (name == "")
		mHasValidName = false;
	else
		mHasValidName = true;
}


void DrumItem::SetUuid(Uuid uuid)
{
	mUuid = uuid;
}


bool DrumItem::GetModifiable(void) const
{
	return mModifiable;
}


void DrumItem::SetModifiable(bool modifiable)
{
	mModifiable = modifiable;
}


bool DrumItem::GetDirty(void) const
{
	return mDirty;
}


void DrumItem::SetDirty(bool dirty)
{
	mDirty = dirty;
}


bool DrumItem::GetHasValidName(void) const
{
	return mHasValidName;
}


DrumItem::Status DrumItem::LoadFromXml(XmlElement* element)
{
	SetDirty(false);

	String name = element->getStringAttribute("name", "");
	if (name == "")
		return kItemNameError;
	String uuid = element->getStringAttribute("uuid", "");
	if (uuid == "")
		return kItemUuidError;

	SetName(name);
	SetUuid(uuid);

	return kNoError;
}


DrumItem::Status DrumItem::SaveToXml(XmlElement* element)
{
	element->setAttribute("name", mName);
	element->setAttribute("uuid", mUuid.toString());
	return kNoError;
}

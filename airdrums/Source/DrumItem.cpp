/*
  ==============================================================================

    DrumItem.cpp
    Created: 30 Mar 2013 12:21:52pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumItem.h"


DrumItem::DrumItem()
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
}


void DrumItem::SetUuid(Uuid uuid)
{
	mUuid = uuid;
}


DrumItem::Status DrumItem::LoadFromXml(XmlElement* element)
{
	String name = element->getStringAttribute("name", "");
	if (name == "")
		return kItemNameError;
	String uuid = element->getStringAttribute("uuid", "");
	if (uuid == "")
		return kItemUuidError;

	mName = name;
	mUuid = uuid;

	return kNoError;
}


DrumItem::Status DrumItem::SaveToXml(XmlElement* element)
{
	element->setAttribute("name", mName);
	element->setAttribute("uuid", mUuid.toString());
	return kNoError;
}

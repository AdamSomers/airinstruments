/*
  ==============================================================================

    DrumPattern.cpp
    Created: 30 Mar 2013 12:38:28pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumPattern.h"
#include "Drums.h"
#include "DrumKit.h"
#include "PatternManager.h"


DrumPattern::DrumPattern() :
	mMidiBuffer(new MidiBuffer)
{
}


DrumPattern::~DrumPattern()
{
}


DrumPattern::Status DrumPattern::LoadFromXml(XmlElement* /*element*/, File& /*directory*/)
{
	return kItemLoadError;
}


DrumPattern::Status DrumPattern::SaveToXml(String fileName, File& directory)
{
	SetName(fileName);

	Drums& drums = Drums::instance();
	double sampleRate = drums.getSampleRate();

	XmlElement main("pattern");
	DrumItem::SaveToXml(&main);
	main.setAttribute("sampleRate", sampleRate);

	SharedPtr<DrumKit> kit = drums.getDrumKit();
	jassert(kit.get() != nullptr);
	XmlElement* kitElement = main.createNewChildElement("kit");
	jassert(kitElement != nullptr);
	kit->DrumItem::SaveToXml(kitElement);

	MidiBuffer::Iterator it(*mMidiBuffer.get());
	const uint8* eventData;
	int eventSize;
	int samplePosition;
	while (it.getNextEvent(eventData, eventSize, samplePosition))
	{
		String eventString = String::toHexString(eventData, eventSize, 0);
		XmlElement* eventElement = main.createNewChildElement("event");
		jassert(eventElement != nullptr);
		eventElement->setAttribute("position", samplePosition);
		eventElement->setAttribute("data", eventString);
	}

	fileName += ".xml";
	File file = directory.getChildFile(fileName);

	if (!main.writeToFile(file, ""))
		return kItemSaveError;

	return kNoError;
}


MidiBuffer& DrumPattern::GetMidiBuffer(void)
{
	return *mMidiBuffer.get();
}

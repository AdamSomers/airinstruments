/*
  ==============================================================================

    DrumKit.cpp
    Created: 25 Mar 2013 1:39:50pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumKit.h"


DrumKit::DrumKit()
{
}


DrumKit::~DrumKit()
{
}


DrumKit::Status DrumKit::LoadFromXml(XmlElement* element, File& directory)
{
	XmlElement* child = element->getChildByName("sample");
	while (child != nullptr)
	{
		SharedPtr<DrumSample> sample(new DrumSample);
		DrumSample::Status status = sample->LoadFromXml(child, directory);
		if (status != DrumSample::kNoError)
			return kSampleLoadError;
		mSamples.push_back(sample);
		child = child->getNextElementWithTagName("sample");
	}

	if (GetSampleCount() == 0)
		return kNoSamplesError;
	return kNoError;
}


SharedPtr<DrumSample> DrumKit::GetSample(int index)
{
	jassert(index < (int) mSamples.size());
	jassert(index >= 0);

	return mSamples.at(index);
}


int DrumKit::GetSampleCount(void)
{
	return mSamples.size();
}


String& DrumKit::GetName(void)
{
	return mName;
}

void DrumKit::LoadTextures()
{
    for (Item drumSample : mSamples)
    {
        drumSample->LoadTextures();
    }
}
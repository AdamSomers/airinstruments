/*
  ==============================================================================

    DrumKit.cpp
    Created: 25 Mar 2013 1:39:50pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "DrumKit.h"
#include "GfxTools.h"

DrumKit::DrumKit()
: mTextureId(0)
{
}


DrumKit::~DrumKit()
{
}


DrumKit::Status DrumKit::LoadFromXml(XmlElement* element, File& directory)
{
	mSamples.clear();

	Status status = DrumItem::LoadFromXml(element);
	if (status != kNoError)
		return status;
    
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }

	XmlElement* child = element->getChildByName("sample");
	while (child != nullptr)
	{
		SharedPtr<DrumSample> sample(new DrumSample);
		DrumSample::Status status = sample->LoadFromXml(child, directory);
		if (status != DrumSample::kNoError)
			return kItemLoadError;
		mSamples.push_back(sample);
		child = child->getNextElementWithTagName("sample");
	}

	if (GetSampleCount() == 0)
		return kItemEmptyError;
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


DrumKit::Status DrumKit::SaveToXml(String /*fileName*/, File& /*directory*/)
{
	return kItemSaveError;
}

void DrumKit::LoadTextures()
{
    for (Item drumSample : mSamples)
    {
        drumSample->LoadTextures();
    }
    
    if (mImage.isValid())
    {
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        GfxTools::loadTextureFromJuceImage(mImage);
    }
}

GLuint DrumKit::GetTexture() const
{
    return mTextureId;
}

const Image& DrumKit::GetImage() const
{
    return mImage;
}

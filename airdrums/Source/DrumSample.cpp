/*
  ==============================================================================

    DrumSample.cpp
    Created: 25 Mar 2013 1:40:21pm
    Author:  Bob Nagy

  ==============================================================================
*/

#include "GfxTools.h"
#include "SkinManager.h"
#include "DrumSample.h"


DrumSample::DrumSample()
: mTextureId(0)
{
}


DrumSample::~DrumSample()
{
}


DrumSample::Status DrumSample::LoadFromXml(XmlElement* element, File& directory)
{
	int note = element->getIntAttribute("note", -1);
	if ((note < 0) || (note > 127))
		return kNoteNumberError;
	String category = element->getStringAttribute("category", "");
	if (category == "")
		return kCategoryError;
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }
	String filename = element->getStringAttribute("file", "");
	if (filename != "")
	{	// This sample has only one implicit layer
		SharedPtr<SampleLayer> layer(new SampleLayer());
		SampleLayer::Status status = layer->LoadFromFile(note, filename, directory);
		if (status != SampleLayer::kNoError)
			return kLayerError;
		mLayers.push_back(layer);
	}
	else
	{	// This sample has one or more explicit layers
		XmlElement* child = element->getChildByName("layer");
		while (child != nullptr)
		{
			SharedPtr<SampleLayer> layer(new SampleLayer());
			SampleLayer::Status status = layer->LoadFromXml(note, child, directory);
			if (status != SampleLayer::kNoError)
				return kLayerError;
			mLayers.push_back(layer);
			child = child->getNextElementWithTagName("layer");
		}
	}

	if (mLayers.size() == 0)
		return kNoLayersError;

	mNoteNumber = note;
	mCategory = category;

	return kNoError;
}


DrumSample::Status DrumSample::CreateFromMemory(const char* data, int size, int note, String category)
{
	if ((note < 0) || (note > 127))
		return kNoteNumberError;
	if (category == "")
		return kCategoryError;

/*
	// TODO :  The built-in kit needs images ?  If so, the image data needs to be made binary and passed in to this method.
    String imageFilename = element->getStringAttribute("image", "");
    if (imageFilename != "")
    {
        File imageFile = directory.getChildFile(imageFilename);
        if (imageFile.existsAsFile())
            mImage = ImageFileFormat::loadFrom(imageFile);
        else
            Logger::outputDebugString("Sample image " + imageFilename + " not found");
    }
*/

	SharedPtr<SampleLayer> layer(new SampleLayer());
	SampleLayer::Status status = layer->CreateFromMemory(data, size, note);
	if (status != SampleLayer::kNoError)
		return kLayerError;
	mLayers.push_back(layer);

	mNoteNumber = note;
	mCategory = category;

	return kNoError;
}


SynthesiserSound::Ptr DrumSample::GetSound(int layer)
{
	if (layer >= (int) mLayers.size())
	{
		jassertfalse;
		return SynthesiserSound::Ptr();
	}

	return mLayers.at(layer)->GetSound();
}


int DrumSample::GetNoteNumber(void)
{
	return mNoteNumber;
}


String& DrumSample::GetCategory(void)
{
	return mCategory;
}


GLuint DrumSample::GetTexture(bool on) const
{
    if (mImage.isValid())
        return mTextureId;
    else {
        String imageName = mCategory;
        if (on)
            imageName += "_on";
        return SkinManager::instance().getSelectedSkin().getTexture(imageName);
    }
}


void DrumSample::LoadTextures()
{
    if (mImage.isValid())
    {
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        GfxTools::loadTextureFromJuceImage(mImage);
    }
}


int DrumSample::GetLayerCount(void)
{
	return mLayers.size();
}

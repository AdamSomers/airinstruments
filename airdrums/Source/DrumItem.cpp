/*
  ==============================================================================

    DrumItem.cpp
    Created: 30 Mar 2013 12:21:52pm
    Author:  Bob Nagy

  ==============================================================================
*/


template <typename Derived>
DrumItem<Derived>::DrumItem() :
	mModifiable(false)
	, mDirty(false)
{
}


template <typename Derived>
DrumItem<Derived>::DrumItem(const DrumItem& source) :
	mName(source.GetName())
	, mUuid(source.GetUuid())
	, mFile(source.GetFile())
	, mModifiable(source.GetModifiable())
	, mDirty(false)
{
}


template <typename Derived>
DrumItem<Derived>::~DrumItem()
{
}


template <typename Derived>
const String& DrumItem<Derived>::GetName(void) const
{
	return mName;
}


template <typename Derived>
const Uuid& DrumItem<Derived>::GetUuid(void) const
{
	return mUuid;
}


template <typename Derived>
void DrumItem<Derived>::SetName(const String& name)
{
	mName = name;
}


template <typename Derived>
void DrumItem<Derived>::SetUuid(const Uuid& uuid)
{
	mUuid = uuid;
}


template <typename Derived>
bool DrumItem<Derived>::GetModifiable(void) const
{
	return mModifiable;
}


template <typename Derived>
void DrumItem<Derived>::SetModifiable(bool modifiable)
{
	mModifiable = modifiable;
}


template <typename Derived>
bool DrumItem<Derived>::GetDirty(void) const
{
	return mDirty;
}


template <typename Derived>
void DrumItem<Derived>::SetDirty(bool dirty)
{
	mDirty = dirty;
	if (!dirty)
		mCleanCopy.reset();
}


template <typename Derived>
const File& DrumItem<Derived>::GetFile(void) const
{
	return mFile;
}


template <typename Derived>
void DrumItem<Derived>::SetFile(const File& file)
{
	mFile = file;
}


template <typename Derived>
typename DrumItem<Derived>::Status DrumItem<Derived>::LoadFromXml(XmlElement* element)
{
	String name = element->getStringAttribute("name", "");
	if (name == "")
		return kItemNameError;
	String uuidString = element->getStringAttribute("uuid", "");
	if (uuidString == "")
		return kItemUuidError;
	bool modifiable = element->getBoolAttribute("modifiable", false);

	Uuid uuid(uuidString);
	SetName(name);
	SetUuid(uuid);
	SetModifiable(modifiable);
	SetDirty(false);

	return kNoError;
}


template <typename Derived>
typename DrumItem<Derived>::Status DrumItem<Derived>::SaveToXml(XmlElement* element)
{
	element->setAttribute("name", mName);
	element->setAttribute("uuid", mUuid.toString());
	element->setAttribute("modifiable", mModifiable);

	SetDirty(false);

	return kNoError;
}


template <typename Derived>
void DrumItem<Derived>::PrepareToModify(void)
{
	if (mCleanCopy.get() != nullptr)
		return;

	Derived* source = (Derived*) (this);
	Derived* derived = new Derived(*source);
	mCleanCopy.reset(derived);
}


template <typename Derived>
void DrumItem<Derived>::RevertToClean(void)
{
	Derived* clean = mCleanCopy.get();
	if (clean == nullptr)
	{
		jassertfalse;
		return;
	}
	Derived* derived = (Derived*) (this);

	derived->RevertToClean(clean);

	SetName(clean->GetName());
	SetUuid(clean->GetUuid());
	SetFile(clean->GetFile());
	SetModifiable(clean->GetModifiable());
	SetDirty(false);
}

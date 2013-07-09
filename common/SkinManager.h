#ifndef h_SkinManager
#define h_SkinManager

#include <iostream>
#include <map>
#include "../JuceLibraryCode/JuceHeader.h"
#include "GfxTools.h"

class Skin
{
public:
    Skin(String skinName);
    const String& getName() const;
    const TextureDescription getTexture(String name) const;
    void addTexture(String name, TextureDescription texture);
private:
    String name;
    std::map<String, TextureDescription> textures;
};

class SkinManager
{
public:
    SkinManager();
    ~SkinManager();
    
    static SkinManager& instance( void )
    {
        static SkinManager s_instance;
        return s_instance;
    }
    
    void loadResources();
    
    const Skin& getSkin(String name) const;
    const Skin& getSelectedSkin() const;
    void setSelectedSkin(const String& name);
    
private:
    std::map<String, Skin> skins;
    String selectedSkinName;
};

#endif // h_SkinManager

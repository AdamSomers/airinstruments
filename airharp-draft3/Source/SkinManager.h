#ifndef h_SkinManager
#define h_SkinManager

#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

class Skin
{
public:
    GLuint background = -1;
    GLuint buttonOn = -1;
    GLuint buttonOff = -1;
    GLuint bezelTop = -1;
    GLuint bezelBottom = -1;
    GLuint string = -1;
    GLuint stringBackground = -1;
private:
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
    
    const Skin& getSkin();
    void setSkinIndex(unsigned int skin);
    
private:
    int currentSkinIndex = 0;
    std::vector<Skin> skins;
};

#endif // h_SkinManager

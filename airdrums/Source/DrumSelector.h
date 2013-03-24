#ifndef h_DrumSelector
#define h_DrumSelector

#include "HUD.h"


class DrumSelector : public HUDView
{
public:
    DrumSelector();
    ~DrumSelector();
    
    // HUDView overrides
    void setup();
    void draw();
    void loadTextures();
    void setBounds(const HUDRect& b);
    
    void setSelection(int sel);
    int getSelection() const { return selection; }

    class Icon : public HUDView
    {
    public:
        Icon(int id);
        ~Icon();
        void setup();
        void draw();
        void loadTextures();
        void setBounds(const HUDRect& b);
    private:
        void updateBounds();
        
        GLBatch batch;
        GLuint textureID;
        int id;
        HUDRect targetBounds;
        HUDRect tempBounds;
        float xStep, yStep, wStep, hStep;
    };
    
private:
    void layoutIcons();

    GLBatch batch;
    GLuint textureID;
    std::vector<Icon*> icons;
    int selection;
    bool needsLayout;
};

#endif // h_DrumSelector

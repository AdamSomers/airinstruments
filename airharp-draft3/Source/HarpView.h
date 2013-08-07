#ifndef h_HarpView
#define h_HarpView

#include <iostream>
#include "StringView.h"

class HarpView
{
public:
    HarpView(int num);
    ~HarpView();
    void draw();
    void update();
    void layoutStrings();
    void addString();
    void removeString();
    void setNumStrings(int numStrings);
    void loadTextures();
    void setVisible(bool shouldBeVisible);
    
    float height;
    int numHarps;
    int harpNum;

private:
    std::vector<StringView*> strings;
    std::vector<StringView*> inactiveStrings;
    std::mutex stringLock;
};

#endif // h_HarpView

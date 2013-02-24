#ifndef h_HarpView
#define h_HarpView

#include <iostream>
#include "StringView.h"

class HarpView
{
public:
    HarpView(int num);
    void draw();
    void update();
    void layoutStrings();
    void addString();
    void removeString();
    void loadTextures();
    
    float height = 2.f;
    int numHarps = 1;
    int harpNum = -1;

private:
    std::vector<StringView*> strings;
    std::vector<StringView*> inactiveStrings;
    std::mutex stringLock;
};

#endif // h_HarpView

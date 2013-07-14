#include "StringView.h"

#include "Main.h"

void StringView::pluck(float position, float velocity/* = 1.f*/, int direction/* = 1*/)
{
    Harp* harp = HarpManager::instance().getHarp(0);
    std::vector<std::string>& scale = harp->getScale();
    int idx = stringNum % scale.size();
    int mult = (stringNum / (float)scale.size());
    int octaveSpan = (ScaleDegrees::getChromatic(scale[scale.size()-1]) / 12) + 1;
    int keyIndex = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("keyIndex", 2);
    int accidental = AirHarpApplication::getInstance()->getProperties().getUserSettings()->getIntValue("accidental", 0);
    int base = baseNotes[keyIndex] + accidental;
    base = base + 12*octaveSpan*mult;
    int note = base + ScaleDegrees::getChromatic(scale[idx]);
    const int bufferSize = 512;
    float buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    int midpoint = position * bufferSize;
    for (int x = 0; x < bufferSize; ++x)
    {
        if (x < midpoint)
            buffer[x] = -x / (float)midpoint;
        else
            buffer[x] = -(1.f - (x - midpoint) / (float)(bufferSize - midpoint));
        
        buffer[x] *= direction;
    }
    
    HarpManager::instance().getHarp(harpNum)->ExciteString(stringNum, note, 127.f * velocity, buffer, bufferSize);
    //HarpManager::instance().getHarp(harpNum)->NoteOn(stringNum, note, 127.f * velocity);
}